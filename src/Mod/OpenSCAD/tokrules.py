# SPDX-License-Identifier: LGPL-2.1-or-later
# -*- coding: utf8 -*-

# ***************************************************************************
# *   Copyright (c) 2012 Keith Sloan <keith@sloan-home.co.uk>               *
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
__title__ = "FreeCAD OpenSCAD Workbench - CSG importer Version 0.5c"
__author__ = "Keith Sloan <keith@sloan-home.co.uk>"
__url__ = ["http://www.sloan-home.co.uk/ImportCSG"]

import re

# ── Token types ────────────────────────────────────────────────────────────

RESERVED = {
    "group",
    "sphere",
    "cylinder",
    "cube",
    "multmatrix",
    "intersection",
    "difference",
    "union",
    "rotate_extrude",
    "linear_extrude",
    "true",
    "false",
    "circle",
    "square",
    "text",
    "polygon",
    "paths",
    "points",
    "undef",
    "polyhedron",
    "triangles",
    "faces",
    "render",
    "surface",
    "subdiv",
    "glide",
    "hull",
    "minkowski",
    "projection",
    "import",
    "color",
    "offset",
    "resize",
}

# Regex-based token specification (order matters)
_TOKEN_SPEC = [
    ("COMMENT", r"//[^\r\n]*"),
    ("STRING", r'"[^"]*"'),
    ("NUMBER", r"[-]?[0-9]*\.?[0-9]+([eE][+-]?[0-9]+)*"),
    ("WORD", r"[$]?[a-zA-Z_]+[0-9]*"),
    ("OBRACE", r"\{"),
    ("EBRACE", r"\}"),
    ("OSQUARE", r"\["),
    ("ESQUARE", r"\]"),
    ("LPAREN", r"\("),
    ("RPAREN", r"\)"),
    ("COMMA", r","),
    ("SEMICOL", r";"),
    ("EQ", r"="),
    ("DOT", r"\."),
    ("MODIFIERBACK", r"%"),
    ("MODIFIERDEBUG", r"\#"),
    ("MODIFIERROOT", r"!"),
    ("MODIFIERDISABLE", r"\*"),
    ("NEWLINE", r"\n+"),
    ("SKIP", r"[ \t\r]+"),
    ("MISMATCH", r"."),
]

_tok_regex = "|".join("(?P<%s>%s)" % pair for pair in _TOKEN_SPEC)
_tok_re = re.compile(_tok_regex)

# Reserved word lookup
RESERVED_SET = RESERVED


def tokenize(input_text):
    """Yield (type, value, lineno) tokens from *input_text*.

    ``WORD`` tokens whose value is a reserved word are emitted with the
    reserved word as their type (e.g. ``"group"``, ``"sphere"``).
    """
    lineno = 1
    for m in _tok_re.finditer(input_text):
        kind = m.lastgroup
        value = m.group()
        if kind == "NEWLINE":
            lineno += value.count("\n")
            continue
        elif kind in ("SKIP", "COMMENT"):
            continue
        elif kind == "MISMATCH":
            raise SyntaxError("Illegal character %r at line %d" % (value, lineno))
        elif kind == "WORD":
            if value.lower() in RESERVED_SET:
                kind = value.lower()
        yield (kind, value, lineno)
