# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2024 Tungsten3D contributors                            *
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

import unittest

import FreeCAD


class TestThread(unittest.TestCase):
    def setUp(self):
        self.Doc = FreeCAD.newDocument("PartDesignTestThread")

    def testThreadOnCylinder(self):
        self.Body = self.Doc.addObject("PartDesign::Body", "Body")
        self.Cylinder = self.Doc.addObject("PartDesign::AdditiveCylinder", "Cylinder")
        self.Body.addObject(self.Cylinder)
        self.Cylinder.Radius = 5.0
        self.Cylinder.Height = 20.0
        self.Doc.recompute()

        self.Thread = self.Doc.addObject("PartDesign::Thread", "Thread")
        self.Thread.ThreadType = 0  # ISO Metric Coarse
        self.Thread.ThreadSize = 5  # M6x1.0
        self.Thread.ThreadDirection = 0  # Right
        self.Thread.ThreadMode = 1  # External
        self.Thread.ThreadLength = 10.0
        self.Body.addObject(self.Thread)
        self.Thread.Base = (self.Cylinder, ["Face3"])
        self.Doc.recompute()

        self.assertFalse(self.Thread.Shape.isNull())
        self.assertFalse(self.Thread.Shape.isEmpty())
        self.assertTrue(len(self.Thread.Shape.Faces) > 0)

    def testThreadProperties(self):
        self.Body = self.Doc.addObject("PartDesign::Body", "Body")
        self.Cylinder = self.Doc.addObject("PartDesign::AdditiveCylinder", "Cylinder")
        self.Body.addObject(self.Cylinder)
        self.Cylinder.Radius = 5.0
        self.Cylinder.Height = 20.0
        self.Doc.recompute()

        self.Thread = self.Doc.addObject("PartDesign::Thread", "Thread")
        self.Thread.ThreadType = 0
        self.Thread.ThreadSize = 5
        self.Thread.ThreadDirection = 0
        self.Thread.ThreadMode = 0  # Auto
        self.Thread.ThreadLength = 10.0
        self.Thread.ThreadPitch = 1.0
        self.Thread.TaperAngle = 0.0
        self.Body.addObject(self.Thread)
        self.Thread.Base = (self.Cylinder, ["Face3"])
        self.Doc.recompute()

        self.assertFalse(self.Thread.Shape.isNull())

    def tearDown(self):
        FreeCAD.closeDocument("PartDesignTestThread")
