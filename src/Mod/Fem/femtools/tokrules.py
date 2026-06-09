# ***************************************************************************
# *   Copyright (c) 2020 Werner Mayer <wmayer[at]users.sourceforge.net>     *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

__title__ = "FEM Utilities"
__author__ = "Werner Mayer"
__url__ = "https://www.freecad.org"

import re
import math

# dictionary of names (for storing variables)
names = {}


# ── Lexer ──────────────────────────────────────────────────────────────────

_TOKEN_SPEC = [
    ("FLOAT", r"\d+\.\d*([eE][+-]?\d+)?"),
    ("INT", r"\d+"),
    ("NAME", r"[a-zA-Z_][a-zA-Z0-9_]*"),
    ("POWER", r"\^"),
    ("PLUS", r"\+"),
    ("MINUS", r"-"),
    ("TIMES", r"\*"),
    ("DIVIDE", r"/"),
    ("EQUALS", r"="),
    ("LPAREN", r"\("),
    ("RPAREN", r"\)"),
    ("COMMENT", r"\#.*"),
    ("NEWLINE", r"\n+"),
    ("SKIP", r"[ \t]+"),
    ("MISMATCH", r"."),
]

_tok_regex = "|".join("(?P<%s>%s)" % pair for pair in _TOKEN_SPEC)
_tok_re = re.compile(_tok_regex)


def _tokenize(input_text):
    """Yield (type, value, lineno) tokens from *input_text*."""
    lineno = 1
    for m in _tok_re.finditer(input_text):
        kind = m.lastgroup
        value = m.group()
        if kind == "NEWLINE":
            lineno += value.count("\n")
            continue
        elif kind == "SKIP" or kind == "COMMENT":
            continue
        elif kind == "MISMATCH":
            raise SyntaxError("Illegal character %r" % value)
        elif kind == "FLOAT":
            yield (kind, float(value), lineno)
        elif kind == "INT":
            yield (kind, int(value), lineno)
        else:
            yield (kind, value, lineno)


# ── Recursive-descent parser ───────────────────────────────────────────────


class _Parser:
    """Parse ``NAME = expression`` or bare ``expression``.

    Operator precedence (low → high):
        +  -       (left)
        *  /       (left)
        ^          (right)
        unary -    (right)
    """

    def __init__(self, tokens):
        self._tokens = list(tokens)
        self._pos = 0

    # ── helpers ─────────────────────────────────────────────────────────
    def _peek(self):
        if self._pos < len(self._tokens):
            return self._tokens[self._pos]
        return None

    def _advance(self):
        tok = self._tokens[self._pos]
        self._pos += 1
        return tok

    def _expect(self, kind):
        tok = self._advance()
        if tok[0] != kind:
            raise SyntaxError("Expected %s, got %s" % (kind, tok[0]))
        return tok

    # ── grammar rules ───────────────────────────────────────────────────

    def parse(self):
        """entry: statement"""
        tok = self._peek()
        if tok is None:
            return
        # NAME EQUALS expression  →  assignment
        if tok[0] == "NAME":
            saved = self._pos
            name_tok = self._advance()
            if self._peek() and self._peek()[0] == "EQUALS":
                self._advance()  # consume =
                val = self._expression()
                names[name_tok[1]] = val
                return
            else:
                # not an assignment – backtrack and parse as expression
                self._pos = saved
        self._expression()

    def _expression(self):
        """expression → term (('+' | '-') term)*"""
        left = self._term()
        while self._peek() and self._peek()[0] in ("PLUS", "MINUS"):
            op = self._advance()[1]
            right = self._term()
            if op == "+":
                left = left + right
            else:
                left = left - right
        return left

    def _term(self):
        """term → power (('*' | '/') power)*"""
        left = self._power()
        while self._peek() and self._peek()[0] in ("TIMES", "DIVIDE"):
            op = self._advance()[1]
            right = self._power()
            if op == "*":
                left = left * right
            else:
                left = left / right
        return left

    def _power(self):
        """power → unary ('^' unary)*   (right-associative)"""
        left = self._unary()
        if self._peek() and self._peek()[0] == "POWER":
            self._advance()
            right = self._power()  # right-associative
            left = left**right
        return left

    def _unary(self):
        """unary → '-' unary | atom"""
        if self._peek() and self._peek()[0] == "MINUS":
            self._advance()
            return -self._unary()
        return self._atom()

    def _atom(self):
        tok = self._peek()
        if tok is None:
            raise SyntaxError("Unexpected end of input")
        kind = tok[0]
        if kind == "FLOAT":
            self._advance()
            return tok[1]
        elif kind == "INT":
            self._advance()
            return tok[1]
        elif kind == "LPAREN":
            self._advance()  # consume (
            val = self._expression()
            self._expect("RPAREN")
            return val
        elif kind == "NAME":
            self._advance()
            try:
                return names[tok[1]]
            except LookupError:
                print("Undefined name '%s'" % tok[1])
                return 0
        else:
            raise SyntaxError("Unexpected token %s" % str(tok))


# ── Public API (same interface as the old ply-based parser) ────────────────


class _YaccWrapper:
    """Mimics the ply.yacc parser object used by task_result_mechanical.py."""

    def parse(self, input="", lexer=None):
        tokens = _tokenize(input)
        p = _Parser(tokens)
        p.parse()


yacc = _YaccWrapper()
