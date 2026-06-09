# SPDX-License-Identifier: LGPL-2.1-or-later

"""
Pure Python YAML parser/dumper module.

Drop-in replacement for pyyaml's safe_load, safe_dump, dump, and load
with SafeLoader. Supports the YAML subset used by FreeCAD material files.
"""

import re


class YAMLError(Exception):
    """Base exception for YAML parsing errors."""

    pass


class SafeLoader:
    """Compatibility class - not used internally but present for API compat."""

    pass


# ---------------------------------------------------------------------------
# Scalar resolution
# ---------------------------------------------------------------------------

_BOOL_FALSE = frozenset({"false", "no", "off"})
_BOOL_TRUE = frozenset({"true", "yes", "on"})
_NULL = frozenset({"null", "~", ""})


def _resolve_scalar(value):
    """Resolve a plain scalar string to its Python type."""
    if value in _NULL:
        return None
    v_lower = value.lower()
    if v_lower in _BOOL_TRUE:
        return True
    if v_lower in _BOOL_FALSE:
        return False
    if re.match(r"^[-+]?0b[0-1_]+$", value):
        return int(value, 2)
    if re.match(r"^[-+]?0o[0-7_]+$", value):
        return int(value, 8)
    if re.match(r"^[-+]?0x[0-9a-fA-F_]+$", value):
        return int(value, 16)
    if re.match(r"^[-+]?(?:[0-9][0-9_]*)?\.[0-9_]*(?:[eE][-+][0-9_]+)?$", value):
        return float(value.replace("_", ""))
    if re.match(r"^[-+]?[0-9][0-9_]*[eE][-+][0-9_]+$", value):
        return float(value.replace("_", ""))
    if re.match(r"^[-+]?[0-9][0-9_]*$", value):
        try:
            return int(value)
        except ValueError:
            pass
    return value


# ---------------------------------------------------------------------------
# Tokenizer - raw line access without skipping blanks
# ---------------------------------------------------------------------------


def _count_indent(line):
    """Return leading space count."""
    return len(line) - len(line.lstrip(" "))


def _strip_comment(line):
    """Remove comments from a line, respecting quoted strings."""
    # Strip BOM character if present at start of line (various encodings)
    if line and ord(line[0]) in (0xFEFF, 0xEF, 0xBF):
        # UTF-8 BOM can appear as \ufeff or as individual bytes ef bb bf
        # on Windows with cp1252 encoding
        if line[0] == "\ufeff":
            line = line[1:]
        elif (
            len(line) >= 3
            and ord(line[0]) == 0xEF
            and ord(line[1]) == 0xBB
            and ord(line[2]) == 0xBF
        ):
            line = line[3:]
    in_single = False
    in_double = False
    i = 0
    while i < len(line):
        c = line[i]
        if c == "\\" and in_double:
            i += 2
            continue
        if c == "'" and not in_double:
            in_single = not in_single
        elif c == '"' and not in_single:
            in_double = not in_double
        elif c == "#" and not in_single and not in_double:
            return line[:i].rstrip()
        i += 1
    return line


class _Tokenizer:
    """Line-oriented tokenizer with raw blank-line preservation."""

    def __init__(self, stream):
        if hasattr(stream, "read"):
            raw = stream.read()
            if isinstance(raw, bytes):
                text = raw.decode("utf-8-sig")
            else:
                text = raw
        elif isinstance(stream, bytes):
            text = stream.decode("utf-8-sig")
        elif isinstance(stream, str):
            text = stream
        else:
            text = str(stream)
        # Strip BOM if present
        if text and text[0] == "\ufeff":
            text = text[1:]
        self._lines = text.split("\n")
        self._pos = 0
        self._n = len(self._lines)

    def is_done(self):
        return self._pos >= self._n

    def peek(self):
        """Return (line, indent, stripped_content) for current line, or None."""
        if self._pos >= self._n:
            return None
        line = self._lines[self._pos]
        indent = _count_indent(line)
        stripped = line.lstrip(" ")
        content = _strip_comment(stripped)
        return line, indent, content

    def advance(self):
        """Consume and return (line, indent, content)."""
        if self._pos >= self._n:
            return None
        line = self._lines[self._pos]
        indent = _count_indent(line)
        stripped = line.lstrip(" ")
        content = _strip_comment(stripped)
        self._pos += 1
        return line, indent, content

    def advance_skip_blanks(self):
        """Consume lines until a non-blank, non-comment-only line is found."""
        while self._pos < self._n:
            line = self._lines[self._pos]
            stripped = line.lstrip(" ")
            content = _strip_comment(stripped)
            if content.strip():
                self._pos += 1
                return line, _count_indent(line), content
            self._pos += 1
        return None

    def peek_skip_blanks(self):
        """Look ahead skipping blanks, returning (line, indent, content) or None."""
        saved = self._pos
        result = self.advance_skip_blanks()
        self._pos = saved
        return result


# ---------------------------------------------------------------------------
# Inline (flow) value parsing
# ---------------------------------------------------------------------------


def _split_flow_items(s):
    """Split comma-separated items, respecting nested brackets and quotes."""
    items = []
    depth = 0
    in_sq = False
    in_dq = False
    cur = []
    for c in s:
        if c == "\\" and in_dq:
            cur.append(c)
            continue
        if c == "'" and not in_dq:
            in_sq = not in_sq
        elif c == '"' and not in_sq:
            in_dq = not in_dq
        if not in_sq and not in_dq:
            if c in "{[":
                depth += 1
            elif c in "}]":
                depth -= 1
            elif c == "," and depth == 0:
                items.append("".join(cur))
                cur = []
                continue
        cur.append(c)
    if cur:
        items.append("".join(cur))
    return items


def _unescape_yaml_string(s):
    """Process YAML escape sequences in a double-quoted string."""
    result = []
    i = 0
    while i < len(s):
        if s[i] == "\\" and i + 1 < len(s):
            next_char = s[i + 1]
            escapes = {
                "n": "\n",
                "t": "\t",
                "r": "\r",
                "b": "\b",
                "f": "\f",
                "0": "\0",
                '"': '"',
                "\\": "\\",
                "'": "'",
                "/": "/",
                " ": " ",
            }
            if next_char == "\n":
                i += 2
                continue
            if next_char in escapes:
                result.append(escapes[next_char])
                i += 2
                continue
            if next_char in "0123456789":
                digits = next_char
                j = i + 2
                while j < len(s) and j < i + 5 and s[j] in "0123456789":
                    digits += s[j]
                    j += 1
                result.append(chr(int(digits)))
                i = j
                continue
            if next_char == "x":
                hex_str = s[i + 2 : i + 4]
                result.append(chr(int(hex_str, 16)))
                i += 4
                continue
            result.append(next_char)
            i += 2
            continue
        result.append(s[i])
        i += 1
    return "".join(result)


def _parse_flow_scalar(s):
    """Parse a flow (inline) scalar: quoted strings, flow seq/mapping, or plain."""
    s = s.strip()
    if not s:
        return ""
    if s.startswith("[") and s.endswith("]"):
        inner = s[1:-1]
        return [_parse_flow_scalar(x) for x in _split_flow_items(inner) if x.strip()]
    if s.startswith("{") and s.endswith("}"):
        inner = s[1:-1]
        result = {}
        for item in _split_flow_items(inner):
            item = item.strip()
            if ":" in item:
                k, v = item.split(":", 1)
                result[k.strip()] = _parse_flow_scalar(v)
        return result
    if s.startswith('"') and s.endswith('"'):
        return _unescape_yaml_string(s[1:-1])
    if s.startswith("'") and s.endswith("'"):
        return s[1:-1]
    return _resolve_scalar(s)


def _extract_key_value(s):
    """
    Extract a key and optional value from a line like 'key: value'.
    Returns (key, rest_of_line_after_colon, has_value).
    """
    s = s.strip()
    if not s:
        return "", None, False

    if s[0] == '"':
        # find closing quote
        i = 1
        while i < len(s):
            if s[i] == "\\":
                i += 2
                continue
            if s[i] == '"':
                break
            i += 1
        key = s[1:i]
        rest = s[i + 1 :].strip()
        if rest.startswith(":"):
            return key, rest[1:].strip(), True
        return key, rest, False

    if s[0] == "'":
        i = s.index("'", 1)
        key = s[1:i]
        rest = s[i + 1 :].strip()
        if rest.startswith(":"):
            return key, rest[1:].strip(), True
        return key, rest, False

    # find colon not inside quotes
    in_sq = False
    in_dq = False
    for i, c in enumerate(s):
        if c == "\\" and in_dq:
            continue
        if c == "'" and not in_dq:
            in_sq = not in_sq
        elif c == '"' and not in_sq:
            in_dq = not in_dq
        elif c == ":" and not in_sq and not in_dq:
            key = s[:i].strip()
            rest = s[i + 1 :].strip()
            return key, rest, True

    return s.strip(), None, False


# ---------------------------------------------------------------------------
# Block scalar parsing
# ---------------------------------------------------------------------------


def _parse_block_scalar(tok, indicator, chomping):
    """
    Parse a literal (|) or folded (>) block scalar.
    Reads the indented body lines and returns the scalar string.
    """
    # Determine the indentation of the body
    # First non-blank line after the indicator determines base indent
    body_indent = None
    lines = []

    while not tok.is_done():
        raw = tok._lines[tok._pos]
        stripped = raw.lstrip(" ")
        content = _strip_comment(stripped)

        if not content.strip():
            # blank line - include it in literal mode, track it
            lines.append(raw)
            tok._pos += 1
            continue

        indent = _count_indent(raw)
        if body_indent is None:
            body_indent = indent
        if indent < body_indent:
            break
        lines.append(raw)
        tok._pos += 1

    if body_indent is None:
        body_indent = 0

    # Strip the body indentation
    body_lines = []
    for raw in lines:
        stripped = raw.lstrip(" ")
        if not stripped or stripped.startswith("#"):
            # blank line - keep as empty
            body_lines.append("")
        elif _count_indent(raw) >= body_indent:
            body_lines.append(raw[body_indent:])
        else:
            body_lines.append("")

    if indicator == "|":
        result = "\n".join(body_lines)
        if chomping == "-":
            result = result.rstrip("\n")
        elif chomping == "+":
            pass
        else:
            result = result.rstrip("\n") + "\n"
    else:
        # folded: join non-empty lines with spaces, preserve blank lines as newlines
        folded = []
        for line in body_lines:
            if line == "":
                folded.append("")
            else:
                if folded and folded[-1] != "":
                    folded[-1] += " " + line
                else:
                    folded.append(line)
        result = "\n".join(folded)
        if chomping == "-":
            result = result.rstrip("\n")
        elif chomping == "+":
            if body_lines and body_lines[-1] == "":
                result += "\n"
        else:
            result = result.rstrip("\n") + "\n"
    return result


# ---------------------------------------------------------------------------
# Block parsing
# ---------------------------------------------------------------------------


def _resolve_value(tok, value_str):
    """
    Given the string after 'key: ', determine its value.
    If empty, check for block scalar or nested child. Returns the value.
    """
    value_str = value_str.strip()

    # Block scalar indicators
    if value_str.startswith("|"):
        chomping = ""
        rest = value_str[1:]
        if rest.startswith("-"):
            chomping = "-"
            rest = rest[1:]
        elif rest.startswith("+"):
            chomping = "+"
            rest = rest[1:]
        return _parse_block_scalar(tok, "|", chomping)

    if value_str.startswith(">"):
        chomping = ""
        rest = value_str[1:]
        if rest.startswith("-"):
            chomping = "-"
            rest = rest[1:]
        elif rest.startswith("+"):
            chomping = "+"
            rest = rest[1:]
        return _parse_block_scalar(tok, ">", chomping)

    # Flow values
    if value_str.startswith("[") or value_str.startswith("{"):
        return _parse_flow_scalar(value_str)

    # Check for the `key: value` where value continues on next line as folded/literal
    # (value_str is just 'key' with the actual value on next lines - handled by caller)

    # Quoted strings - always return as-is (string), don't resolve types
    if value_str and len(value_str) >= 2:
        if value_str[0] == '"' and value_str[-1] == '"':
            return _unescape_yaml_string(value_str[1:-1])
        if value_str[0] == "'" and value_str[-1] == "'":
            return value_str[1:-1]

    # Flow scalar
    if value_str:
        return _parse_flow_scalar(value_str)

    return None


def _parse_mapping(tok, indent):
    """Parse a YAML mapping at the given indentation level."""
    result = {}

    while not tok.is_done():
        peeked = tok.peek_skip_blanks()
        if peeked is None:
            break
        _, p_indent, p_content = peeked

        if p_indent < indent:
            break
        if p_indent > indent:
            break

        if p_content.startswith("---"):
            tok.advance_skip_blanks()
            continue

        if p_content.startswith("- "):
            break

        # Parse key: value
        line, li_indent, content = tok.advance_skip_blanks()
        if line is None:
            break

        content = content.strip()
        if not content or content.startswith("#"):
            continue
        if content.startswith("---"):
            continue

        key, value_str, has_value = _extract_key_value(content)

        if has_value:
            if value_str is None:
                value_str = ""
            value_str = value_str.strip()
            if value_str:
                result[key] = _resolve_value(tok, value_str)
            else:
                # Value is on subsequent lines - peek to see what kind
                child = _parse_child_of_key(tok, indent)
                result[key] = child
        else:
            # No colon - bare key
            child = _parse_child_of_key(tok, indent)
            if child is not None:
                result[key] = child
            else:
                result[key] = None

    return result


def _parse_sequence(tok, indent):
    """Parse a YAML sequence at the given indentation level."""
    result = []

    while not tok.is_done():
        peeked = tok.peek_skip_blanks()
        if peeked is None:
            break
        _, p_indent, p_content = peeked

        if p_indent < indent:
            break
        if p_indent > indent:
            break

        if p_content.startswith("---"):
            tok.advance_skip_blanks()
            continue

        if not (p_content.startswith("- ") or p_content == "-"):
            break

        # Consume the '- ' line
        line, li_indent, content = tok.advance_skip_blanks()
        if line is None:
            break

        content = content.strip()
        if content == "-":
            item_content = ""
        elif content.startswith("- "):
            item_content = content[2:]
        else:
            item_content = content

        # The mapping content after '- ' starts at li_indent + 2
        key_indent = li_indent + 2

        # Check if item has inline value
        key, value_str, has_value = _extract_key_value(item_content)

        if has_value:
            if value_str is not None and value_str.strip():
                result.append({key: _resolve_value(tok, value_str.strip())})
            else:
                # - key: (value on next lines)
                # Peek to determine if next content is child or sibling
                child = _parse_child_of_key(tok, indent)
                result.append({key: child})
        elif item_content.startswith("[") or item_content.startswith("{"):
            result.append(_parse_flow_scalar(item_content))
        elif item_content:
            result.append(_parse_flow_scalar(item_content))
        else:
            child = _parse_child_of_key(tok, key_indent)
            if child is not None:
                result.append(child)
            else:
                result.append(None)

    return result


def _parse_child_of_key(tok, parent_indent):
    """
    Parse the child content under a key that had an empty value.
    The next line could be:
    - At parent_indent: sibling key (return None)
    - At parent_indent + 2: child mapping or sequence
    - At parent_indent + 4+: deeper nested structure
    """
    peeked = tok.peek_skip_blanks()
    if peeked is None:
        return None

    _, p_indent, p_content = peeked

    # If next line is at same indent, it's a sibling key, not a child
    if p_indent <= parent_indent:
        return None

    # Skip document markers
    if p_content.startswith("---"):
        tok.advance_skip_blanks()
        return _parse_child_of_key(tok, parent_indent)

    # Determine type based on content
    if p_content.startswith("- ") or p_content == "-":
        return _parse_sequence(tok, p_indent)
    else:
        return _parse_mapping(tok, p_indent)


def _skip_document_start(tok):
    """Skip --- document start marker if present."""
    peeked = tok.peek_skip_blanks()
    if peeked is not None:
        _, _, content = peeked
        if content.strip() == "---" or content.strip().startswith("--- "):
            tok.advance_skip_blanks()


# ---------------------------------------------------------------------------
# Public API: safe_load
# ---------------------------------------------------------------------------


def safe_load(stream):
    """
    Parse a YAML document from a string or file-like object.
    Returns a Python dict, list, or scalar value.
    """
    tok = _Tokenizer(stream)
    _skip_document_start(tok)

    if tok.is_done():
        return None

    peeked = tok.peek_skip_blanks()
    if peeked is None:
        return None

    _, indent, content = peeked

    # Determine root type
    if content.startswith("- ") or content == "-":
        return _parse_sequence(tok, indent)
    return _parse_mapping(tok, indent)


def load(stream, Loader=None):
    """
    Compatibility function. Loader parameter is ignored; safe parsing is always used.
    """
    return safe_load(stream)


# ---------------------------------------------------------------------------
# Dumper
# ---------------------------------------------------------------------------

_NEEDS_QUOTE_RE = re.compile(
    r"^[~]|^null$|^None$|^true$|^false$|^yes$|^no$|^on$|^off$"
    r"|^[-+]?[0-9]"
    r"|[&*?!|>%`@#:\[\]{}\s]"
    r"|^$"
)


def _needs_quoting(s):
    """Return True if a string needs quoting in YAML."""
    if not isinstance(s, str):
        return True
    if not s:
        return False
    if _NEEDS_QUOTE_RE.search(s):
        return True
    try:
        int(s)
        return True
    except ValueError:
        pass
    try:
        float(s)
        return True
    except ValueError:
        pass
    return False


def _format_scalar(value):
    """Format a Python value as a YAML scalar."""
    if value is None:
        return "null"
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, int):
        return str(value)
    if isinstance(value, float):
        if value != value:
            return ".nan"
        if value == float("inf"):
            return ".inf"
        if value == float("-inf"):
            return "-.inf"
        s = repr(value)
        if "e" in s.lower():
            return s
        return str(value)
    if isinstance(value, str):
        if not value:
            return '""'
        if _needs_quoting(value) or "'" in value:
            escaped = (
                value.replace("\\", "\\\\")
                .replace('"', '\\"')
                .replace("\n", "\\n")
                .replace("\r", "\\r")
                .replace("\t", "\\t")
            )
            return '"' + escaped + '"'
        return value
    return str(value)


def _dump_block(data, indent_level, indent_str="  "):
    """Recursively dump data to YAML block format."""
    prefix = indent_str * indent_level
    lines = []

    if isinstance(data, dict):
        if not data:
            lines.append(prefix + "{}")
        else:
            for key, value in data.items():
                fk = _format_scalar(key)
                if isinstance(value, dict):
                    if not value:
                        lines.append(prefix + fk + ": {}")
                    else:
                        lines.append(prefix + fk + ":")
                        lines.extend(_dump_block(value, indent_level + 1, indent_str))
                elif isinstance(value, list):
                    lines.append(prefix + fk + ":")
                    lines.extend(_dump_block(value, indent_level + 1, indent_str))
                elif value is None:
                    lines.append(prefix + fk + ": null")
                else:
                    lines.append(prefix + fk + ": " + _format_scalar(value))

    elif isinstance(data, list):
        if not data:
            lines.append(prefix + "[]")
        else:
            for item in data:
                if isinstance(item, dict):
                    if not item:
                        lines.append(prefix + "- {}")
                    else:
                        first_key = True
                        for key, value in item.items():
                            fk = _format_scalar(key)
                            if first_key:
                                if isinstance(value, dict):
                                    if not value:
                                        lines.append(prefix + "- " + fk + ": {}")
                                    else:
                                        lines.append(prefix + "- " + fk + ":")
                                        lines.extend(
                                            _dump_block(
                                                value, indent_level + 1, indent_str
                                            )
                                        )
                                elif isinstance(value, list):
                                    lines.append(prefix + "- " + fk + ":")
                                    lines.extend(
                                        _dump_block(value, indent_level + 1, indent_str)
                                    )
                                elif value is None:
                                    lines.append(prefix + "- " + fk + ": null")
                                else:
                                    lines.append(
                                        prefix
                                        + "- "
                                        + fk
                                        + ": "
                                        + _format_scalar(value)
                                    )
                                first_key = False
                            else:
                                # subsequent keys of the same dict item
                                vprefix = prefix + "  "
                                if isinstance(value, dict):
                                    if not value:
                                        lines.append(vprefix + fk + ": {}")
                                    else:
                                        lines.append(vprefix + fk + ":")
                                        lines.extend(
                                            _dump_block(
                                                value, indent_level + 2, indent_str
                                            )
                                        )
                                elif isinstance(value, list):
                                    lines.append(vprefix + fk + ":")
                                    lines.extend(
                                        _dump_block(value, indent_level + 2, indent_str)
                                    )
                                elif value is None:
                                    lines.append(vprefix + fk + ": null")
                                else:
                                    lines.append(
                                        vprefix + fk + ": " + _format_scalar(value)
                                    )
                elif isinstance(item, list):
                    lines.append(prefix + "-")
                    lines.extend(_dump_block(item, indent_level + 1, indent_str))
                elif item is None:
                    lines.append(prefix + "- null")
                else:
                    lines.append(prefix + "- " + _format_scalar(item))
    else:
        lines.append(prefix + _format_scalar(data))

    return lines


def _dump_flow(data):
    """Dump data to YAML flow (inline) format."""
    if isinstance(data, dict):
        if not data:
            return "{}"
        items = []
        for k, v in data.items():
            items.append(_format_scalar(k) + ": " + _dump_flow(v))
        return "{ " + ", ".join(items) + " }"
    if isinstance(data, list):
        if not data:
            return "[]"
        return "[" + ", ".join(_dump_flow(item) for item in data) + "]"
    return _format_scalar(data)


def safe_dump(data, default_flow_style=None):
    """
    Serialize Python data to a YAML string.

    default_flow_style:
        None or False: block style (default, most readable)
        True: flow style (inline)
    """
    if default_flow_style is True:
        return _dump_flow(data) + "\n"
    lines = _dump_block(data, 0)
    return "\n".join(lines) + "\n"


def dump(data, default_flow_style=None):
    """
    Alias for safe_dump. We don't support unsafe features.
    """
    return safe_dump(data, default_flow_style=default_flow_style)
