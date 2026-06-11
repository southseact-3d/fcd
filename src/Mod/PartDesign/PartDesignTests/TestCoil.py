# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2024 Tungsten CAD contributors                           *
# *                                                                         *
# *   This file is part of FreeCAD.                                         *
# *                                                                         *
# *   FreeCAD is free software: you can redistribute it and/or modify it    *
# *   under the terms of the GNU Lesser General Public License as           *
# *   published by the Free Software Foundation, either version 2.1 of the  *
# *   License, or (at your option) any later version.                       *
# *                                                                         *
# *   FreeCAD is distributed in the hope that it will be useful, but        *
# *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
# *   Lesser General Public License for more details.                       *
# *                                                                         *
# *   You should have received a copy of the GNU Lesser General Public      *
# *   License along with FreeCAD. If not, see                               *
# *   <https://www.gnu.org/licenses/>.                                      *
# *                                                                         *
# ***************************************************************************

import unittest

import FreeCAD
from FreeCAD import Base

"""Test various coil configurations"""


class TestCoil(unittest.TestCase):
    """Test various coil configurations"""

    def setUp(self):
        self.Doc = FreeCAD.newDocument("PartDesignTestCoil")

    def testBasicCylindricalCoil(self):
        """Test basic cylindrical coil with circular cross-section"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0  # Revolution+Height
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 1  # On Center
        coil.LeftHanded = False
        coil.Reversed = False

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testRevolutionPitchMode(self):
        """Test coil in Revolution+Pitch mode"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 1  # Revolution+Pitch
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Pitch = 5.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testHeightPitchMode(self):
        """Test coil in Height+Pitch mode"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 2  # Height+Pitch
        coil.Radius = 10.0
        coil.Height = 25.0
        coil.Pitch = 5.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testSpiralMode(self):
        """Test coil in Spiral mode (flat 2D spiral)"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 3  # Spiral
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Pitch = 2.0
        coil.SectionSize = 1.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertTrue(coil.Shape.isValid())

    def testSquareCrossSection(self):
        """Test coil with square cross-section"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 1  # Square
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testTriangularExternalCrossSection(self):
        """Test coil with triangular external cross-section"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 2  # Triangular External
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testTriangularInternalCrossSection(self):
        """Test coil with triangular internal cross-section"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 3  # Triangular Internal
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testInsideSectionPosition(self):
        """Test coil with Inside section position"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 0  # Inside

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testOutsideSectionPosition(self):
        """Test coil with Outside section position"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 2  # Outside

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testConicalCoil(self):
        """Test coil with taper angle"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.Angle = 15.0  # 15 degree taper
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testLeftHandedCoil(self):
        """Test left-handed coil"""
        body = self.Doc.addObject("PartDesign::Body", "Body")
        coil = body.newObject("PartDesign::AdditiveCoil", "Coil")

        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.LeftHanded = True

        self.Doc.recompute()
        self.assertEqual(len(coil.Shape.Solids), 1)
        self.assertTrue(coil.Shape.isValid())

    def testSubtractiveCoil(self):
        """Test subtractive coil"""
        body = self.Doc.addObject("PartDesign::Body", "Body")

        # First create a base cylinder
        base = body.newObject("PartDesign::AdditiveCylinder", "Base")
        base.Radius = 20.0
        base.Height = 30.0
        self.Doc.recompute()

        # Then create a subtractive coil
        coil = body.newObject("PartDesign::SubtractiveCoil", "Coil")
        coil.Mode = 0
        coil.Radius = 10.0
        coil.Turns = 5.0
        coil.Height = 25.0
        coil.SectionSize = 2.0
        coil.SectionShape = 0  # Circular
        coil.SectionPosition = 1  # On Center

        self.Doc.recompute()
        self.assertTrue(coil.Shape.isValid())

    def tearDown(self):
        FreeCAD.closeDocument(self.Doc.Name)
