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
# *   Acknowledgements:                                                     *
# *                                                                         *
# *     Thanks to shoogen on the FreeCAD forum and Peter Li                 *
# *     for programming advice and some code.                               *
# *                                                                         *
# *                                                                         *
# ***************************************************************************
__title__ = "FreeCAD OpenSCAD Workbench - CSG importer"
__author__ = "Keith Sloan <keith@sloan-home.co.uk>"
__url__ = ["http://www.sloan-home.co.uk/ImportCSG"]

printverbose = False

import io
import os

import xml.sax

import FreeCAD
import Part
import Draft

from OpenSCADFeatures import *
from OpenSCADUtils import *


params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD")
printverbose = params.GetBool("printVerbose", False)

if FreeCAD.GuiUp:
    gui = True
else:
    if printverbose:
        print("FreeCAD Gui not present.")
    gui = False

hassetcolor = []
alreadyhidden = []
original_root_objects = []

import tokrules
from builtins import open as pyopen

translate = FreeCAD.Qt.translate


def shallHide(subject):
    for obj in subject.OutListRecursive:
        if "Matrix_Union" in str(obj.FullName):
            return False
        if "Extrude" in str(obj.FullName):
            return True
    return False


def setColorRecursively(obj, color, transp):
    """
    For some reason a part made by cutting or fusing other parts do not have a color
    unless its constituents are also colored. This code sets colors for those
    constituents unless already set elsewhere.
    """
    obj.ViewObject.ShapeColor = color
    obj.ViewObject.Transparency = transp
    boolean_features = [
        "Part::Fuse",
        "Part::MultiFuse",
        "Part::Cut",
        "Part::Common",
        "Part::MultiCommon",
    ]
    if obj.TypeId in boolean_features:
        for currentObject in obj.OutList:
            if printverbose:
                print(f"Fixing up colors for: {currentObject.FullName}")
            if currentObject not in hassetcolor:
                setColorRecursively(currentObject, color, transp)


def fixVisibility():
    for root_object in FreeCAD.ActiveDocument.RootObjects:
        if root_object not in original_root_objects:
            root_object.ViewObject.Visibility = True
            for obj in root_object.OutListRecursive:
                obj.ViewObject.Visibility = False


def open(filename):
    "called when freecad opens a file."
    global doc
    global pathName
    docname = os.path.splitext(os.path.basename(filename))[0]
    doc = FreeCAD.newDocument(docname)
    if filename.lower().endswith(".scad"):
        tmpfile = callopenscad(filename)
        pathName = os.path.dirname(os.path.normpath(filename))
        processcsg(tmpfile)
        try:
            os.unlink(tmpfile)
        except OSError:
            pass
    else:
        pathName = os.path.dirname(os.path.normpath(filename))
        processcsg(filename)
    return doc


def insert(filename, docname):
    "called when freecad imports a file"
    global doc
    global pathName
    groupname_unused = os.path.splitext(os.path.basename(filename))[0]
    try:
        doc = FreeCAD.getDocument(docname)
        for obj in doc.RootObjects:
            original_root_objects.append(obj)
    except NameError:
        doc = FreeCAD.newDocument(docname)
    if filename.lower().endswith(".scad"):
        tmpfile = callopenscad(filename)
        pathName = os.path.dirname(os.path.normpath(filename))
        processcsg(tmpfile)
        try:
            os.unlink(tmpfile)
        except OSError:
            pass
    else:
        pathName = os.path.dirname(os.path.normpath(filename))
        processcsg(filename)


# ── Helper functions (used by the parser) ──────────────────────────────────


def placeholder(name, children, arguments):
    from OpenSCADFeatures import OpenSCADPlaceholder

    newobj = doc.addObject("Part::FeaturePython", name)
    OpenSCADPlaceholder(newobj, children, str(arguments))
    if gui:
        if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD").GetBool(
            "useViewProviderTree"
        ):
            from OpenSCADFeatures import ViewProviderTree

            ViewProviderTree(newobj.ViewObject)
        else:
            newobj.ViewObject.Proxy = 0
    return newobj


def CGALFeatureObj(name, children, arguments=[]):
    myobj = doc.addObject("Part::FeaturePython", name)
    CGALFeature(myobj, name, children, str(arguments))
    if gui:
        for subobj in children:
            subobj.ViewObject.hide()
        if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD").GetBool(
            "useViewProviderTree"
        ):
            from OpenSCADFeatures import ViewProviderTree

            ViewProviderTree(myobj.ViewObject)
        else:
            myobj.ViewObject.Proxy = 0
    return myobj


def checkObjShape(obj):
    if printverbose:
        print("Check Object Shape")
    if hasattr(obj, "Shape"):
        if obj.Shape.isNull():
            if printverbose:
                print("Shape is Null - recompute")
            obj.recompute()
        if obj.Shape.isNull():
            print(f"Recompute failed : {obj.Name}")
    else:
        if hasattr(obj, "Name"):
            print(f"obj {obj.Name} has no Shape")
        else:
            print(f"obj {obj} has no Name & Shape")


def fuse(lst, name):
    global doc
    if printverbose:
        print("Fuse")
    if printverbose:
        print(lst)
    if len(lst) == 0:
        myfuse = placeholder("group", [], "{}")
    elif len(lst) == 1:
        return lst[0]
    elif len(lst) > 2:
        if printverbose:
            print("Multi Fuse")
        myfuse = doc.addObject("Part::MultiFuse", name)
        myfuse.Shapes = lst
        if gui:
            for subobj in myfuse.Shapes:
                subobj.ViewObject.hide()
    else:
        if printverbose:
            print("Single Fuse")
        myfuse = doc.addObject("Part::Fuse", name)
        myfuse.Base = lst[0]
        myfuse.Tool = lst[1]
        checkObjShape(myfuse.Base)
        checkObjShape(myfuse.Tool)
        myfuse.Shape = myfuse.Base.Shape.fuse(myfuse.Tool.Shape)
        if gui:
            myfuse.Base.ViewObject.hide()
            myfuse.Tool.ViewObject.hide()
    myfuse.Placement = FreeCAD.Placement()
    return myfuse


def process_rotate_extrude(obj, angle):
    newobj = doc.addObject("Part::FeaturePython", "RefineRotateExtrude")
    RefineShape(newobj, obj)
    if gui:
        if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD").GetBool(
            "useViewProviderTree"
        ):
            from OpenSCADFeatures import ViewProviderTree

            ViewProviderTree(newobj.ViewObject)
        else:
            newobj.ViewObject.Proxy = 0
        obj.ViewObject.hide()
    myrev = doc.addObject("Part::Revolution", "RotateExtrude")
    myrev.Source = newobj
    myrev.Axis = (0.00, 1.00, 0.00)
    myrev.Base = (0.00, 0.00, 0.00)
    myrev.Angle = angle
    myrev.Placement = FreeCAD.Placement(FreeCAD.Vector(), FreeCAD.Rotation(0, 0, 90))
    if gui:
        newobj.ViewObject.hide()
    return myrev


def process_rotate_extrude_prism(obj, angle, n):
    newobj = doc.addObject("Part::FeaturePython", "PrismaticToroid")
    PrismaticToroid(newobj, obj, angle, n)
    newobj.Placement = FreeCAD.Placement(FreeCAD.Vector(), FreeCAD.Rotation(0, 0, 90))
    if gui:
        if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD").GetBool(
            "useViewProviderTree"
        ):
            from OpenSCADFeatures import ViewProviderTree

            ViewProviderTree(newobj.ViewObject)
        else:
            newobj.ViewObject.Proxy = 0
        obj.ViewObject.hide()
    return newobj


def process_linear_extrude(obj, h):
    newobj = doc.addObject("Part::FeaturePython", "RefineLinearExtrude")
    RefineShape(newobj, obj)
    if gui:
        if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD").GetBool(
            "useViewProviderTree"
        ):
            from OpenSCADFeatures import ViewProviderTree

            ViewProviderTree(newobj.ViewObject)
        else:
            newobj.ViewObject.Proxy = 0
        obj.ViewObject.hide()
    mylinear = doc.addObject("Part::Extrusion", "LinearExtrude")
    mylinear.Base = newobj
    mylinear.Dir = (0, 0, h)
    mylinear.Placement = FreeCAD.Placement()
    mylinear.Solid = False
    if gui:
        newobj.ViewObject.hide()
    return mylinear


def process_linear_extrude_with_transform(base, height, twist, scale):
    newobj = doc.addObject("Part::FeaturePython", "transform_extrude")
    Twist(newobj, base, height, -twist, scale)
    if gui:
        if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/OpenSCAD").GetBool(
            "useViewProviderTree"
        ):
            from OpenSCADFeatures import ViewProviderTree

            ViewProviderTree(newobj.ViewObject)
        else:
            newobj.ViewObject.Proxy = 0
    return newobj


def process_import_file(fname, ext, layer):
    if printverbose:
        print("Importing : " + fname + "." + ext + " Layer : " + layer)
    if ext.lower() in reverseimporttypes()["Mesh"]:
        obj = process_mesh_file(fname, ext)
    elif ext.lower() == "dxf":
        obj = processDXF(fname, layer)
    elif ext.lower() == "svg":
        obj = processSVG(fname, ext)
    else:
        raise ValueError("Unsupported file extension %s" % ext)
    return obj


def processSVG(fname, ext):
    from importSVG import svgHandler

    if printverbose:
        print("SVG Handler")
    doc = FreeCAD.ActiveDocument
    docSVG = FreeCAD.newDocument(fname + "_tmp")
    FreeCAD.ActiveDocument = docSVG
    parser = xml.sax.make_parser()
    parser.setFeature(xml.sax.handler.feature_external_ges, False)
    parser.setContentHandler(svgHandler())
    parser._cont_handler.doc = docSVG
    filename = os.path.join(pathName, fname + "." + ext)
    parser.parse(pyopen(filename))
    shapes = []
    for obj in FreeCAD.ActiveDocument.Objects:
        if printverbose:
            print(obj.Name)
        if printverbose:
            print(obj.Shape)
        shapes.append(obj.Shape)
    FreeCAD.closeDocument(docSVG.Name)
    FreeCAD.ActiveDocument = doc
    obj = doc.addObject("Part::Feature", fname)
    obj.Shape = Part.Compound(shapes)
    return obj


def process_mesh_file(fname, ext):
    import Mesh
    import Part

    fullname = fname + "." + ext
    filename = os.path.join(pathName, fullname)
    objname = os.path.split(fname)[1]
    mesh1 = doc.getObject(objname)
    if not mesh1:
        Mesh.insert(filename)
        mesh1 = doc.getObject(objname)
    if mesh1 is not None:
        if gui:
            mesh1.ViewObject.hide()
        sh = Part.Shape()
        sh.makeShapeFromMesh(mesh1.Mesh.Topology, 0.1)
        solid = Part.Solid(sh)
        obj = doc.addObject("Part::Feature", "Mesh")
        solid = solid.removeSplitter()
        if solid.Volume < 0:
            solid.complement()
        obj.Shape = solid
    else:
        FreeCAD.Console.PrintError(
            "Mesh not imported %s.%s %s\n" % (objname, ext, filename)
        )
        import Part

        obj = doc.addObject("Part::Feature", "FailedMeshImport")
        obj.Shape = Part.Compound([])
    return obj


def processTextCmd(t):
    from OpenSCADUtils import callopenscadstring

    tmpfilename = callopenscadstring(t, "dxf")
    from OpenSCAD2Dgeom import importDXFface

    face = importDXFface(tmpfilename, None, None)
    obj = doc.addObject("Part::Feature", "text")
    obj.Shape = face
    try:
        os.unlink(tmpfilename)
    except OSError:
        pass
    return obj


def processDXF(fname, layer):
    global doc
    global pathName
    from OpenSCAD2Dgeom import importDXFface

    if printverbose:
        print("Process DXF file")
    if printverbose:
        print("File Name : " + fname)
    if printverbose:
        print("Layer : " + layer)
    if printverbose:
        print("PathName : " + pathName)
    dxfname = fname + ".dxf"
    filename = os.path.join(pathName, dxfname)
    shortname = os.path.split(fname)[1]
    if printverbose:
        print("DXF Full path : " + filename)
    face = importDXFface(filename, layer, doc)
    obj = doc.addObject("Part::Feature", "dxf_%s_%s" % (shortname, layer or "all"))
    obj.Shape = face
    if printverbose:
        print("DXF Diagnostics")
    if printverbose:
        print(obj.Shape.ShapeType)
    if printverbose:
        print("Closed : " + str(obj.Shape.isClosed()))
    if printverbose:
        print(obj.Shape.check())
    if printverbose:
        print([w.isClosed() for w in obj.Shape.Wires])
    return obj


def processSTL(fname):
    if printverbose:
        print("Process STL file")


def center(obj, x, y, z):
    obj.Placement = FreeCAD.Placement(
        FreeCAD.Vector(-x / 2.0, -y / 2.0, -z / 2.0), FreeCAD.Rotation(0, 0, 0, 1)
    )


def myPolygon(n, r1):
    import math

    if printverbose:
        print("My Polygon")
    angle = math.pi * 2 / n
    nodes = [FreeCAD.Vector(r1, 0, 0)]
    for i in range(n - 1):
        th = (i + 1) * angle
        nodes.append(FreeCAD.Vector(r1 * math.cos(th), r1 * math.sin(th), 0))
    nodes.append(nodes[0])
    polygonwire = Part.makePolygon(nodes)
    polygon = doc.addObject("Part::Feature", "Polygon")
    polygon.Shape = Part.Face(polygonwire)
    return polygon


def addString(t, s, params_dict):
    return t + ", " + s + ' = "' + params_dict[s] + '"'


def addValue(t, v, params_dict):
    return t + ", " + v + " = " + params_dict[v]


def convert_points_list_to_vector(l):
    v = []
    for i in l:
        if printverbose:
            print(i)
        v.append(FreeCAD.Vector(i[0], i[1]))
    if printverbose:
        print(v)
    return v


def make_face(v1, v2, v3):
    wire = Part.makePolygon([v1, v2, v3, v1])
    face = Part.Face(wire)
    return face


# ── Recursive descent parser for CSG ──────────────────────────────────────

_MODIFIER_TOKENS = {"MODIFIERBACK", "MODIFIERDEBUG", "MODIFIERROOT", "MODIFIERDISABLE"}
_OPERATION_KEYWORDS = {
    "difference",
    "intersection",
    "union",
    "rotate_extrude",
    "linear_extrude",
    "import",
    "surface",
    "projection",
    "hull",
    "minkowski",
    "offset",
    "resize",
    "glide",
    "subdiv",
}
_PART_KEYWORDS = {
    "sphere",
    "cylinder",
    "cube",
    "circle",
    "square",
    "text",
    "polygon",
    "polyhedron",
}


class _Parser:
    """Recursive descent parser for OpenSCAD CSG format."""

    def __init__(self, tokens):
        self._tokens = list(tokens)
        self._pos = 0

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
            raise SyntaxError("Expected %s, got %s at line %d" % (kind, tok[0], tok[2]))
        return tok

    # ── Grammar rules ───────────────────────────────────────────────────

    def parse(self):
        """block_list : statement | block_list statement | statementwithmod | block_list statementwithmod"""
        result = []
        while self._peek() is not None:
            tok = self._peek()
            if tok[0] in _MODIFIER_TOKENS:
                item = self._parse_statementwithmod()
            else:
                item = self._parse_statement()
            if item is not None:
                if isinstance(item, list):
                    result.extend(item)
                else:
                    result.append(item)
        return result

    def _parse_statementwithmod(self):
        """anymodifier statement"""
        modifier = self._advance()[0]
        obj = self._parse_statement()
        if hasattr(obj, "Label"):
            obj.Label = modifier + obj.Label
        return obj

    def _parse_statement(self):
        """part | operation | multmatrix_action | group_action1 | group_action2 | color_action | render_action | not_supported"""
        tok = self._peek()
        if tok is None:
            return None
        if tok[0] in _PART_KEYWORDS:
            return self._parse_part()
        elif tok[0] == "multmatrix":
            return self._parse_multmatrix_action()
        elif tok[0] == "group":
            return self._parse_group_action()
        elif tok[0] == "color":
            return self._parse_color_action()
        elif tok[0] == "render":
            return self._parse_render_action()
        elif tok[0] in ("glide", "subdiv"):
            return self._parse_not_supported()
        elif tok[0] in _OPERATION_KEYWORDS:
            return self._parse_operation()
        else:
            raise SyntaxError("Unexpected token %s at line %d" % (str(tok), tok[2]))

    def _parse_operation(self):
        tok = self._peek()
        if tok[0] == "difference":
            return self._parse_difference_action()
        elif tok[0] == "intersection":
            return self._parse_intersection_action()
        elif tok[0] == "union":
            return self._parse_union_action()
        elif tok[0] == "rotate_extrude":
            return self._parse_rotate_extrude()
        elif tok[0] == "linear_extrude":
            return self._parse_linear_extrude_with_transform()
        elif tok[0] == "import":
            return self._parse_import_file1()
        elif tok[0] == "surface":
            return self._parse_surface_action()
        elif tok[0] == "projection":
            return self._parse_projection_action()
        elif tok[0] == "hull":
            return self._parse_hull_action()
        elif tok[0] == "minkowski":
            return self._parse_minkowski_action()
        elif tok[0] == "offset":
            return self._parse_offset_action()
        elif tok[0] == "resize":
            return self._parse_resize_action()
        raise SyntaxError("Unexpected operation %s" % str(tok))

    def _parse_part(self):
        tok = self._peek()
        if tok[0] == "sphere":
            return self._parse_sphere_action()
        elif tok[0] == "cylinder":
            return self._parse_cylinder_action()
        elif tok[0] == "cube":
            return self._parse_cube_action()
        elif tok[0] == "circle":
            return self._parse_circle_action()
        elif tok[0] == "square":
            return self._parse_square_action()
        elif tok[0] == "text":
            return self._parse_text_action()
        elif tok[0] == "polygon":
            return self._parse_polygon_action()
        elif tok[0] == "polyhedron":
            return self._parse_polyhedron_action()
        raise SyntaxError("Unexpected part %s" % str(tok))

    # ── Group actions ───────────────────────────────────────────────────

    def _parse_group_action(self):
        self._expect("group")
        self._expect("LPAREN")
        tok = self._peek()
        if tok[0] == "RPAREN":
            self._advance()
            if self._peek()[0] == "SEMICOL":
                self._advance()
                if printverbose:
                    print("Group2")
                return []
            self._expect("OBRACE")
            block = self._parse_block_list()
            self._expect("EBRACE")
            if printverbose:
                print("Group")
            if block is None or len(block) == 0:
                return []
            if len(block) > 1:
                if printverbose:
                    print("Fuse Group")
                for obj in block:
                    checkObjShape(obj)
                return [fuse(block, "Group")]
            else:
                if printverbose:
                    print(f"Group {block} type {type(block)}")
                checkObjShape(block)
                return block
        raise SyntaxError("Expected ) after group()")

    # ── Render action ───────────────────────────────────────────────────

    def _parse_render_action(self):
        self._expect("render")
        self._expect("LPAREN")
        self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("Render (ignored)")
        return block

    # ── Color action ────────────────────────────────────────────────────

    def _parse_color_action(self):
        import math

        self._expect("color")
        self._expect("LPAREN")
        vec = self._parse_vector()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("Color")
        color = tuple([float(f) for f in vec[:3]])
        transp = 100 - int(math.floor(100 * float(vec[3])))
        if gui:
            for obj in block:
                if shallHide(obj):
                    if "Group" in obj.FullName:
                        obj.ViewObject.Visibility = False
                        alreadyhidden.append(obj)
                setColorRecursively(obj, color, transp)
                hassetcolor.append(obj)
        return block

    # ── Boolean operations ──────────────────────────────────────────────

    def _parse_difference_action(self):
        self._expect("difference")
        self._expect("LPAREN")
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("difference")
        if printverbose:
            print(len(block))
        if printverbose:
            print(block)
        if len(block) == 0:
            placeholder("group", [], "{}")
            return []
        elif len(block) == 1:
            return block
        else:
            mycut = doc.addObject("Part::Cut", "difference")
            mycut.Base = block[0]
            if len(block) > 2:
                if printverbose:
                    print("Need to Fuse Extra First")
                mycut.Tool = fuse(block[1:], "union")
            else:
                mycut.Tool = block[1]
                checkObjShape(mycut.Tool)
            if gui:
                mycut.Base.ViewObject.hide()
                mycut.Tool.ViewObject.hide()
            if printverbose:
                print("Push Resulting Cut")
            if printverbose:
                print("End Cut")
            return [mycut]

    def _parse_intersection_action(self):
        self._expect("intersection")
        self._expect("LPAREN")
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("intersection")
        if len(block) > 2:
            if printverbose:
                print("Multi Common")
            mycommon = doc.addObject("Part::MultiCommon", "intersection")
            mycommon.Shapes = block
            if gui:
                for subobj in mycommon.Shapes:
                    subobj.ViewObject.hide()
        elif len(block) == 2:
            if printverbose:
                print("Single Common")
            mycommon = doc.addObject("Part::Common", "intersection")
            mycommon.Base = block[0]
            mycommon.Tool = block[1]
            checkObjShape(mycommon.Base)
            checkObjShape(mycommon.Tool)
            if gui:
                mycommon.Base.ViewObject.hide()
                mycommon.Tool.ViewObject.hide()
        elif len(block) == 1:
            mycommon = block[0]
        else:
            mycommon = placeholder("group", [], "{}")
        mycommon.Shape = mycommon.Base.Shape.common(mycommon.Tool.Shape)
        if printverbose:
            print("End Intersection")
        return [mycommon]

    def _parse_union_action(self):
        self._expect("union")
        self._expect("LPAREN")
        self._expect("RPAREN")
        tok = self._peek()
        if tok[0] == "SEMICOL":
            self._advance()
            if printverbose:
                print("empty union")
            newpart = fuse([], "union")
            if printverbose:
                print("Push Union Result")
            if printverbose:
                print("End Union")
            return [newpart]
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("union")
        newpart = fuse(block, "union")
        if printverbose:
            print("Push Union Result")
        if printverbose:
            print("End Union")
        return [newpart]

    # ── Extrude / rotate ────────────────────────────────────────────────

    def _parse_rotate_extrude(self):
        self._expect("rotate_extrude")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        tok = self._peek()
        if tok[0] == "SEMICOL":
            self._advance()
            if printverbose:
                print("Rotate Extrude File")
            angle = 360.0
            if "angle" in kwargs:
                angle = float(kwargs["angle"])
            filen, ext = kwargs["file"].rsplit(".", 1)
            obj = process_import_file(filen, ext, kwargs["layer"])
            n = int(round(float(kwargs["$fn"])))
            fnmax = FreeCAD.ParamGet(
                "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
            ).GetInt("useMaxFN", 16)
            if n < 3 or fnmax != 0 and n > fnmax:
                return [process_rotate_extrude(obj, angle)]
            else:
                return [process_rotate_extrude_prism(obj, angle, n)]
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("Rotate Extrude")
        angle = 360.0
        if "angle" in kwargs:
            angle = float(kwargs["angle"])
        n = int(round(float(kwargs["$fn"])))
        fnmax = FreeCAD.ParamGet(
            "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
        ).GetInt("useMaxFN", 16)
        if len(block) > 1:
            part = fuse(block, "Rotate Extrude Union")
        else:
            part = block[0]
        if n < 3 or fnmax != 0 and n > fnmax:
            result = [process_rotate_extrude(part, angle)]
        else:
            result = [process_rotate_extrude_prism(part, angle, n)]
        if printverbose:
            print("End Rotate Extrude")
        return result

    def _parse_linear_extrude_with_transform(self):
        self._expect("linear_extrude")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("Linear Extrude With Transform")
        h = float(kwargs["height"])
        if printverbose:
            print("Height : ", h)
        s = [1.0, 1.0]
        t = 0.0
        if "scale" in kwargs:
            if isinstance(kwargs["scale"], str):
                s = [float(kwargs["scale"]), float(kwargs["scale"])]
            else:
                s = [float(kwargs["scale"][0]), float(kwargs["scale"][1])]
            if printverbose:
                print("Scale: " + str(s))
        if "twist" in kwargs:
            t = float(kwargs["twist"])
            if printverbose:
                print("Twist : ", t)
        if len(block) == 0:
            return []
        if len(block) > 1:
            obj = fuse(block, "Linear Extrude Union")
        else:
            obj = block[0]
        checkObjShape(obj)
        if t != 0.0 or s[0] != 1.0 or s[1] != 1.0:
            newobj = process_linear_extrude_with_transform(obj, h, t, s)
        else:
            newobj = process_linear_extrude(obj, h)
        if kwargs.get("center", "false") == "true":
            center(newobj, 0, 0, h)
        if gui:
            obj.ViewObject.hide()
        if printverbose:
            print("End Linear Extrude with Transform")
        return [newobj]

    # ── Import / surface / projection ───────────────────────────────────

    def _parse_import_file1(self):
        self._expect("import")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Import File")
        filen, ext = kwargs["file"].rsplit(".", 1)
        if printverbose:
            print("End Import File")
        return [process_import_file(filen, ext, kwargs["layer"])]

    def _parse_surface_action(self):
        self._expect("surface")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Surface")
        obj = doc.addObject("Part::Feature", "surface")
        obj.Shape, xoff, yoff = makeSurfaceVolume(kwargs["file"])
        if kwargs.get("center", "false") == "true":
            center(obj, xoff, yoff, 0.0)
        if printverbose:
            print("End surface")
        return [obj]

    def _parse_projection_action(self):
        self._expect("projection")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("Projection")
        doc.recompute()
        block[0].Shape.tessellate(0.05)
        bbox = block[0].Shape.BoundBox
        for shape in block:
            shape.Shape.tessellate(0.05)
            bbox.add(shape.Shape.BoundBox)
        plane = doc.addObject("Part::Plane", "xy_plane_used_for_projection")
        plane.Length = bbox.XLength
        plane.Width = bbox.YLength
        plane.Placement = FreeCAD.Placement(
            FreeCAD.Vector(bbox.XMin, bbox.YMin, 0), FreeCAD.Rotation()
        )
        if gui:
            plane.ViewObject.hide()
        if kwargs["cut"] == "true":
            obj = doc.addObject("Part::MultiCommon", "projection_cut")
            if len(block) > 1:
                subobj = [fuse(block, "projection_cut_implicit_group")]
            else:
                subobj = block
            obj.Shapes = [plane] + subobj
            if gui:
                subobj[0].ViewObject.hide()
            return [obj]
        else:
            if gui and not FreeCAD.ParamGet(
                "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
            ).GetBool("usePlaceholderForUnsupported"):
                from PySide import QtGui

                QtGui.QMessageBox.critical(
                    None,
                    translate("OpenSCAD", "Unsupported Function") + " : projection",
                    translate("OpenSCAD", "Press OK"),
                )
            else:
                return [placeholder("projection", block, kwargs)]

    # ── Hull / minkowski / offset / resize / not_supported ──────────────

    def _parse_hull_action(self):
        self._expect("hull")
        self._expect("LPAREN")
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        return [CGALFeatureObj("hull", block)]

    def _parse_minkowski_action(self):
        self._expect("minkowski")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        return [CGALFeatureObj("minkowski", block, kwargs)]

    def _parse_offset_action(self):
        self._expect("offset")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        subobj = None
        if len(block) == 0:
            newobj = placeholder("group", [], "{}")
        elif len(block) == 1:
            subobj = block[0]
        else:
            subobj = fuse(block, "Offset Union")
        if "r" in kwargs:
            offset = float(kwargs["r"])
        if "delta" in kwargs:
            offset = float(kwargs["delta"])
        checkObjShape(subobj)
        if subobj.Shape.Volume == 0:
            newobj = doc.addObject("Part::Offset2D", "Offset2D")
            newobj.Source = subobj
            newobj.Value = offset
            if "r" in kwargs:
                newobj.Join = 0
            else:
                newobj.Join = 2
        else:
            newobj = doc.addObject("Part::Offset", "offset")
            newobj.Shape = subobj[0].Shape.makeOffset(offset)
        newobj.Document.recompute()
        if gui:
            subobj.ViewObject.hide()
        return [newobj]

    def _parse_resize_action(self):
        self._expect("resize")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        new_size = kwargs["newsize"]
        auto = kwargs["auto"]
        block[0].recompute()
        if block[0].Shape.isNull():
            doc.recompute()
        block[0].Shape.tessellate(0.05)
        old_bbox = block[0].Shape.BoundBox
        old_size = [old_bbox.XLength, old_bbox.YLength, old_bbox.ZLength]
        for r in range(0, 3):
            if auto[r] == "1":
                new_size[r] = new_size[0]
            if new_size[r] == "0":
                new_size[r] = str(old_size[r])
        transform_matrix = FreeCAD.Matrix()
        scale = FreeCAD.Vector(
            float(new_size[0]) / old_size[0],
            float(new_size[1]) / old_size[1],
            float(new_size[2]) / old_size[2],
        )
        transform_matrix.scale(scale)
        new_part = doc.addObject("Part::FeaturePython", "Matrix Deformation")
        new_part.Shape = block[0].Shape.transformGeometry(transform_matrix)
        if gui:
            if FreeCAD.ParamGet(
                "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
            ).GetBool("useViewProviderTree"):
                from OpenSCADFeatures import ViewProviderTree

                ViewProviderTree(new_part.ViewObject)
            else:
                new_part.ViewObject.Proxy = 0
            block[0].ViewObject.hide()
        return [new_part]

    def _parse_not_supported(self):
        keyword = self._advance()[0]
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if gui and not FreeCAD.ParamGet(
            "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
        ).GetBool("usePlaceholderForUnsupported"):
            from PySide import QtGui

            QtGui.QMessageBox.critical(
                None,
                translate("OpenSCAD", "Unsupported Function") + " : " + keyword,
                translate("OpenSCAD", "Press OK"),
            )
            return []
        else:
            return [placeholder(keyword, block, kwargs)]

    # ── Primitive actions ───────────────────────────────────────────────

    def _parse_sphere_action(self):
        self._expect("sphere")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Sphere : ", kwargs)
        r = float(kwargs["r"])
        mysphere = doc.addObject("Part::Sphere", "sphere")
        mysphere.Radius = r
        if printverbose:
            print("Push Sphere")
        if printverbose:
            print("End Sphere")
        return [mysphere]

    def _parse_cylinder_action(self):
        self._expect("cylinder")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Cylinder")
        tocenter = kwargs.get("center", "false")
        h = float(kwargs["h"])
        r1 = float(kwargs["r1"])
        r2 = float(kwargs["r2"])
        n = int(round(float(kwargs["$fn"])))
        fnmax = FreeCAD.ParamGet(
            "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
        ).GetInt("useMaxFN", 16)
        if printverbose:
            print(kwargs)
        if h > 0:
            if r1 == r2 and r1 > 0:
                if printverbose:
                    print("Make Cylinder")
                if n < 3 or fnmax != 0 and n > fnmax:
                    mycyl = doc.addObject("Part::Cylinder", "cylinder")
                    mycyl.Height = h
                    mycyl.Radius = r1
                else:
                    if printverbose:
                        print("Make Prism")
                    mycyl = doc.addObject("Part::Prism", "prism")
                    mycyl.Polygon = n
                    mycyl.Circumradius = r1
                    mycyl.Height = h
            elif r1 != r2:
                if n < 3 or fnmax != 0 and n > fnmax:
                    if printverbose:
                        print("Make Cone")
                    mycyl = doc.addObject("Part::Cone", "cylinder")
                    mycyl.Height = h
                    mycyl.Radius1 = r1
                    mycyl.Radius2 = r2
                else:
                    if printverbose:
                        print("Make Frustum")
                    mycyl = doc.addObject("Part::FeaturePython", "frustum")
                    Frustum(mycyl, r1, r2, n, h)
                    if gui:
                        if FreeCAD.ParamGet(
                            "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
                        ).GetBool("useViewProviderTree"):
                            from OpenSCADFeatures import ViewProviderTree

                            ViewProviderTree(mycyl.ViewObject)
                        else:
                            mycyl.ViewObject.Proxy = 0
            else:
                FreeCAD.Console.PrintWarning("cylinder with radius zero\n")
                mycyl = doc.addObject("Part::Feature", "emptycyl")
                mycyl.Shape = Part.Compound([])
        else:
            FreeCAD.Console.PrintWarning("cylinder with height <= zero\n")
            mycyl = doc.addObject("Part::Feature", "emptycyl")
            mycyl.Shape = Part.Compound([])
        if printverbose:
            print("Center = ", tocenter)
        if tocenter == "true":
            center(mycyl, 0, 0, h)
        if printverbose:
            print("End Cylinder")
        return [mycyl]

    def _parse_cube_action(self):
        self._expect("cube")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        global doc
        l, w, h = [float(str1) for str1 in kwargs["size"]]
        if l > 0 and w > 0 and h > 0:
            if printverbose:
                print("cube : ", kwargs)
            mycube = doc.addObject("Part::Box", "cube")
            mycube.Length = l
            mycube.Width = w
            mycube.Height = h
        else:
            FreeCAD.Console.PrintWarning("cube with radius zero\n")
            mycube = doc.addObject("Part::Feature", "emptycube")
            mycube.Shape = Part.Compound([])
        if kwargs.get("center", "false") == "true":
            center(mycube, l, w, h)
        if printverbose:
            print("End Cube")
        return [mycube]

    def _parse_circle_action(self):
        self._expect("circle")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Circle : " + str(kwargs))
        r = float(kwargs["r"])
        if r == 0:
            r = 0.00001
        n = int(kwargs["$fn"])
        fnmax = FreeCAD.ParamGet(
            "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
        ).GetInt("useMaxFN", 16)
        import Draft

        if n == 0 or fnmax != 0 and n >= fnmax:
            mycircle = FreeCAD.ActiveDocument.addObject(
                "Part::Part2DObjectPython", "circle"
            )
            Draft._Circle(mycircle)
            mycircle.Radius = r
            mycircle.MakeFace = True
            mycircle = Draft.makeCircle(r, face=True)
            FreeCAD.ActiveDocument.recompute()
        else:
            mycircle = FreeCAD.ActiveDocument.addObject(
                "Part::Part2DObjectPython", "polygon"
            )
            Draft._Polygon(mycircle)
            mycircle.FacesNumber = n
            mycircle.Radius = r
            mycircle.DrawMode = "inscribed"
            mycircle.MakeFace = True
        if gui:
            Draft._ViewProviderDraft(mycircle.ViewObject)
        if printverbose:
            print("Push Circle")
        return [mycircle]

    def _parse_square_action(self):
        self._expect("square")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Square")
        size = kwargs["size"]
        x = float(size[0])
        y = float(size[1])
        mysquare = doc.addObject("Part::Plane", "square")
        mysquare.Length = x
        mysquare.Width = y
        if kwargs.get("center", "false") == "true":
            center(mysquare, x, y, 0)
        return [mysquare]

    def _parse_text_action(self):
        self._expect("text")
        self._expect("LPAREN")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if kwargs["text"] == "" or kwargs["text"] == " ":
            return []
        t = 'text ( text="' + kwargs["text"] + '"'
        t = addValue(t, "size", kwargs)
        t = addString(t, "spacing", kwargs)
        t = addString(t, "font", kwargs)
        t = addString(t, "direction", kwargs)
        t = addString(t, "language", kwargs)
        if "script" in kwargs:
            t = addString(t, "script", kwargs)
        else:
            t += ', script="latin"'
        t = addString(t, "halign", kwargs)
        t = addString(t, "valign", kwargs)
        t = addValue(t, "$fn", kwargs)
        t = addValue(t, "$fa", kwargs)
        t = addValue(t, "$fs", kwargs)
        t = t + ");"
        FreeCAD.Console.PrintMessage("textmsg : " + t + "\n")
        return [processTextCmd(t)]

    def _parse_polygon_action(self):
        self._expect("polygon")
        self._expect("LPAREN")
        self._expect("points")
        self._expect("EQ")
        self._expect("OSQUARE")
        points = self._parse_points_list_2d()
        self._expect("ESQUARE")
        self._expect("COMMA")
        self._expect("paths")
        self._expect("EQ")
        tok = self._peek()
        if tok[0] == "undef":
            self._advance()
            self._expect("COMMA")
            kwargs = self._parse_keywordargument_list()
            self._expect("RPAREN")
            self._expect("SEMICOL")
            if printverbose:
                print("Polygon")
            if printverbose:
                print(points)
            v = convert_points_list_to_vector(points)
            mypolygon = doc.addObject("Part::Feature", "polygon")
            if printverbose:
                print("Make Parts")
            v.append(v[0])
            parts = Part.makePolygon(v)
            if printverbose:
                print("update object")
            mypolygon.Shape = Part.Face(parts)
            return [mypolygon]
        else:
            self._expect("OSQUARE")
            path_set = self._parse_path_set()
            self._expect("ESQUARE")
            self._expect("COMMA")
            kwargs = self._parse_keywordargument_list()
            self._expect("RPAREN")
            self._expect("SEMICOL")
            if printverbose:
                print("Polygon with Path")
            if printverbose:
                print(points)
            v = convert_points_list_to_vector(points)
            if printverbose:
                print("Path Set List")
            if printverbose:
                print(path_set)
            for i in path_set:
                if printverbose:
                    print(i)
                mypolygon = doc.addObject("Part::Feature", "wire")
                path_list = []
                for j in i:
                    j = int(j)
                    if printverbose:
                        print(j)
                    path_list.append(v[j])
                path_list.append(v[int(i[0])])
                if printverbose:
                    print("Path List")
                if printverbose:
                    print(path_list)
                wire = Part.makePolygon(path_list)
                mypolygon.Shape = Part.Face(wire)
            return [mypolygon]

    def _parse_polyhedron_action(self):
        self._expect("polyhedron")
        self._expect("LPAREN")
        self._expect("points")
        self._expect("EQ")
        self._expect("OSQUARE")
        points = self._parse_points_list_3d()
        self._expect("ESQUARE")
        self._expect("COMMA")
        tok = self._advance()
        label = tok[1]  # "faces" or "triangles"
        self._expect("EQ")
        self._expect("OSQUARE")
        faces_data = self._parse_path_set()
        self._expect("ESQUARE")
        self._expect("COMMA")
        kwargs = self._parse_keywordargument_list()
        self._expect("RPAREN")
        self._expect("SEMICOL")
        if printverbose:
            print("Polyhedron Points")
        v = []
        for i in points:
            if printverbose:
                print(i)
            v.append(FreeCAD.Vector(float(i[0]), float(i[1]), float(i[2])))
        if printverbose:
            print(v)
            print("Polyhedron " + label)
            print(faces_data)
        faces_list = []
        mypolyhed = doc.addObject("Part::Feature", "polyhedron")
        for i in faces_data:
            if printverbose:
                print(i)
            v2 = FreeCAD.Vector
            pp = [v2(v[k]) for k in i]
            pp.append(pp[0])
            try:
                w = Part.makePolygon(pp)
                f = Part.Face(w)
            except Exception:
                secWireList = w.Edges[:]
                f = Part.makeFilledFace(Part.__sortEdges__(secWireList))
            faces_list.append(f)
        shell = Part.makeShell(faces_list)
        solid = Part.Solid(shell).removeSplitter()
        if solid.Volume < 0:
            solid.reverse()
        mypolyhed.Shape = solid
        return [mypolyhed]

    # ── Multmatrix ──────────────────────────────────────────────────────

    def _parse_multmatrix_action(self):
        self._expect("multmatrix")
        self._expect("LPAREN")
        matrix = self._parse_matrix()
        self._expect("RPAREN")
        self._expect("OBRACE")
        block = self._parse_block_list()
        self._expect("EBRACE")
        if printverbose:
            print("MultMatrix")
        if printverbose:
            print("Multmatrix")
        if printverbose:
            print(matrix)
        if gui and block:
            parentcolor = block[0].ViewObject.ShapeColor
            parenttransparency = block[0].ViewObject.Transparency
        m1l = sum(matrix, [])
        if any("x" in me for me in m1l):
            m1l = [float.fromhex(me) for me in m1l]
            matrixisrounded = False
        elif max((len(me) for me in m1l)) >= 14:
            m1l = [float(me) for me in m1l]
            m1l = [(0 if (abs(me) < 1e-15) else me) for me in m1l]
            matrixisrounded = False
        else:
            m1l = [round(float(me), 12) for me in m1l]
            matrixisrounded = True
        transform_matrix = FreeCAD.Matrix(*tuple(m1l))
        if printverbose:
            print(transform_matrix)
        if printverbose:
            print("Apply Multmatrix")
        if len(block) == 0:
            part = placeholder("group", [], "{}")
        elif len(block) > 1:
            part = fuse(block, "Matrix Union")
        else:
            part = block[0]
        if isspecialorthogonalpython(fcsubmatrix(transform_matrix)):
            if printverbose:
                print("special orthogonal")
            if matrixisrounded:
                if printverbose:
                    print("rotation rounded")
                plm = FreeCAD.Placement(transform_matrix)
                plm = FreeCAD.Placement(plm.Base, roundrotation(plm.Rotation))
                part.Placement = plm.multiply(part.Placement)
            else:
                part.Placement = FreeCAD.Placement(transform_matrix).multiply(
                    part.Placement
                )
            new_part = part
        elif isrotoinversionpython(fcsubmatrix(transform_matrix)):
            if printverbose:
                print("orthogonal and inversion")
            cmat, axisvec = decomposerotoinversion(transform_matrix)
            new_part = doc.addObject("Part::Mirroring", "mirr_%s" % part.Name)
            new_part.Source = part
            new_part.Normal = axisvec
            if matrixisrounded:
                if printverbose:
                    print("rotation rounded")
                plm = FreeCAD.Placement(cmat)
                new_part.Placement = FreeCAD.Placement(
                    plm.Base, roundrotation(plm.Rotation)
                )
            else:
                new_part.Placement = FreeCAD.Placement(cmat)
            new_part.Label = "mirrored %s" % part.Label
            if gui:
                part.ViewObject.hide()
        elif FreeCAD.ParamGet(
            "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
        ).GetBool("useMultmatrixFeature"):
            from OpenSCADFeatures import MatrixTransform

            new_part = doc.addObject("Part::FeaturePython", "Matrix Deformation")
            MatrixTransform(new_part, transform_matrix, part)
            if gui:
                if FreeCAD.ParamGet(
                    "User parameter:BaseApp/Preferences/Mod/OpenSCAD"
                ).GetBool("useViewProviderTree"):
                    from OpenSCADFeatures import ViewProviderTree

                    ViewProviderTree(new_part.ViewObject)
                else:
                    new_part.ViewObject.Proxy = 0
                part.ViewObject.hide()
        else:
            if printverbose:
                print("Transform Geometry")
            part.recompute()
            if part.Shape.isNull():
                doc.recompute()
            new_part = doc.addObject("Part::Feature", "Matrix Deformation")
            new_part.Shape = part.Shape.transformGeometry(transform_matrix)
            if gui:
                part.ViewObject.hide()
        if gui and block:
            new_part.ViewObject.ShapeColor = parentcolor
            new_part.ViewObject.Transparency = parenttransparency
        if printverbose:
            print("Multmatrix applied")
        return [new_part]

    # ── Data structure parsers ──────────────────────────────────────────

    def _parse_block_list(self):
        result = []
        while self._peek() is not None and self._peek()[0] != "EBRACE":
            tok = self._peek()
            if tok[0] in _MODIFIER_TOKENS:
                item = self._parse_statementwithmod()
            else:
                item = self._parse_statement()
            if item is not None:
                if isinstance(item, list):
                    result.extend(item)
                else:
                    result.append(item)
        return result

    def _parse_2d_point(self):
        self._expect("OSQUARE")
        x = float(self._expect("NUMBER")[1])
        self._expect("COMMA")
        y = float(self._expect("NUMBER")[1])
        self._expect("ESQUARE")
        if printverbose:
            print("2D Point")
        return [x, y]

    def _parse_points_list_2d(self):
        point = self._parse_2d_point()
        result = [point]
        while self._peek() is not None and self._peek()[0] == "COMMA":
            self._advance()
            if self._peek()[0] == "ESQUARE":
                break
            point = self._parse_2d_point()
            result.append(point)
        return result

    def _parse_3d_point(self):
        self._expect("OSQUARE")
        x = self._expect("NUMBER")[1]
        self._expect("COMMA")
        y = self._expect("NUMBER")[1]
        self._expect("COMMA")
        z = self._expect("NUMBER")[1]
        self._expect("ESQUARE")
        if printverbose:
            print("3d point")
        return [x, y, z]

    def _parse_points_list_3d(self):
        point = self._parse_3d_point()
        result = [point]
        while self._peek() is not None and self._peek()[0] == "COMMA":
            self._advance()
            if self._peek()[0] == "ESQUARE":
                break
            point = self._parse_3d_point()
            result.append(point)
        return result

    def _parse_path_points(self):
        result = [int(self._expect("NUMBER")[1])]
        while self._peek() is not None and self._peek()[0] == "NUMBER":
            result.append(int(self._advance()[1]))
        return result

    def _parse_path_list(self):
        self._expect("OSQUARE")
        points = self._parse_path_points()
        self._expect("ESQUARE")
        return points

    def _parse_path_set(self):
        result = [self._parse_path_list()]
        while self._peek() is not None and self._peek()[0] == "COMMA":
            self._advance()
            if self._peek()[0] == "ESQUARE":
                break
            result.append(self._parse_path_list())
        return result

    def _parse_vector(self):
        self._expect("OSQUARE")
        x = self._expect("NUMBER")[1]
        self._expect("COMMA")
        y = self._expect("NUMBER")[1]
        self._expect("COMMA")
        z = self._expect("NUMBER")[1]
        self._expect("COMMA")
        w = self._expect("NUMBER")[1]
        self._expect("ESQUARE")
        if printverbose:
            print("Vector")
        return [x, y, z, w]

    def _parse_matrix(self):
        self._expect("OSQUARE")
        v1 = self._parse_vector()
        self._expect("COMMA")
        v2 = self._parse_vector()
        self._expect("COMMA")
        v3 = self._parse_vector()
        self._expect("COMMA")
        v4 = self._parse_vector()
        self._expect("ESQUARE")
        if printverbose:
            print("Matrix")
        return [v1, v2, v3, v4]

    def _parse_size_vector(self):
        self._expect("OSQUARE")
        x = self._expect("NUMBER")[1]
        self._expect("COMMA")
        y = self._expect("NUMBER")[1]
        self._expect("COMMA")
        z = self._expect("NUMBER")[1]
        self._expect("ESQUARE")
        if printverbose:
            print("size vector")
        return [x, y, z]

    def _parse_boolean(self):
        tok = self._advance()
        return tok[1]

    def _parse_stripped_string(self):
        tok = self._expect("STRING")
        return tok[1].strip('"')

    def _parse_keywordargument(self):
        key_tok = self._advance()
        key = key_tok[1]
        self._expect("EQ")
        tok = self._peek()
        if tok[0] in ("true", "false"):
            val = self._parse_boolean()
        elif tok[0] == "NUMBER":
            val = self._advance()[1]
        elif tok[0] == "OSQUARE":
            # Could be size_vector, vector, or 2d_point
            saved = self._pos
            # Try to distinguish: peek ahead for pattern
            # size_vector: [N,N,N]
            # vector: [N,N,N,N]
            # 2d_point: [N,N]
            # We need to count commas to distinguish
            peek_pos = self._pos + 1  # skip OSQUARE
            comma_count = 0
            while (
                peek_pos < len(self._tokens) and self._tokens[peek_pos][0] != "ESQUARE"
            ):
                if self._tokens[peek_pos][0] == "COMMA":
                    comma_count += 1
                peek_pos += 1
            if comma_count == 3:
                val = self._parse_vector()
            elif comma_count == 2:
                val = self._parse_size_vector()
            else:
                val = self._parse_2d_point()
        elif tok[0] == "STRING":
            val = self._parse_stripped_string()
        else:
            raise SyntaxError("Unexpected token in keyword argument: %s" % str(tok))
        if printverbose:
            print((key, val))
        return (key, val)

    def _parse_keywordargument_list(self):
        first = self._parse_keywordargument()
        result = {first[0]: first[1]}
        while self._peek() is not None and self._peek()[0] == "COMMA":
            self._advance()
            if self._peek()[0] == "RPAREN":
                break
            kw = self._parse_keywordargument()
            result[kw[0]] = kw[1]
        return result


# ── Main entry point ───────────────────────────────────────────────────────


def processcsg(filename):
    global doc

    if printverbose:
        print("ImportCSG Version 0.6a")
    if printverbose:
        print("Start Lex")

    f = io.open(filename, "r", encoding="utf8")
    text = f.read()
    f.close()

    tokens = list(tokrules.tokenize(text))

    if printverbose:
        print("End Lex")
    if printverbose:
        print("Load Parser")

    parser = _Parser(tokens)
    result = parser.parse()

    if printverbose:
        print("End Parser")
        print(result)
    if gui:
        fixVisibility()
    hassetcolor.clear()
    alreadyhidden.clear()
    FreeCAD.Console.PrintMessage("End processing CSG file\n")
    doc.recompute()
