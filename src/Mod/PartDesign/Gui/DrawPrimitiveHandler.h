// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 liamh <liamh[at]users.sourceforge.net>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef PARTDESIGN_DrawPrimitiveHandler_H
#define PARTDESIGN_DrawPrimitiveHandler_H

#include <memory>
#include <QObject>
#include <QComboBox>
#include <Gui/SelectionObserver.h>
#include <Gui/ViewProvider.h>
#include <Base/Vector3D.h>
#include <App/DocumentObject.h>
#include <Mod/PartDesign/App/FeaturePrimitive.h>

class View3DInventorViewer;
class SoSeparator;
class SoCoordinate3;

namespace PartDesignGui
{

/**
 * State machine for the interactive primitive drawing workflow
 */
enum class DrawPrimitiveState
{
    SelectPlane,        // Waiting for user to select a plane or planar face
    DrawProfile,        // Drawing the 2D profile (rectangle or circle)
    AdjustHeight,       // Adjusting the height via drag handle
    Finished            // Drawing complete
};

/**
 * Handler for interactive primitive drawing (Box and Cylinder)
 * 
 * Workflow:
 * 1. User selects a plane or planar face
 * 2. User draws the 2D profile (two-corner rect for Box, center+drag for Cylinder)
 * 3. User adjusts the height via drag handle or dialog
 * 4. Shape is created
 */
class DrawPrimitiveHandler : public QObject, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    /**
     * Primitive type to draw
     */
    enum class PrimitiveType
    {
        Box,
        Cylinder
    };

    DrawPrimitiveHandler(PrimitiveType type, App::DocumentObject* feature, QObject* parent = nullptr);
    ~DrawPrimitiveHandler() override;

    /**
     * Start the interactive drawing workflow
     */
    void start();

    /**
     * Cancel the interactive drawing workflow
     */
    void cancel();

    /**
     * Get the current state
     */
    DrawPrimitiveState getState() const { return _state; }

    /**
     * Get the selected plane normal
     */
    Base::Vector3d getPlaneNormal() const { return _planeNormal; }

    /**
     * Get the selected plane origin
     */
    Base::Vector3d getPlaneOrigin() const { return _planeOrigin; }

Q_SIGNALS:
    /**
     * Emitted when the plane is selected
     */
    void planeSelected(const Base::Vector3d& origin, const Base::Vector3d& normal);

    /**
     * Emitted when the profile is drawn
     */
    void profileDrawn(double width, double height);

    /**
     * Emitted when the drawing is finished
     */
    void finished();

    /**
     * Emitted when the drawing is cancelled
     */
    void cancelled();

protected:
    /**
     * Handle selection changes (from SelectionObserver)
     */
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    /**
     * Handle plane selection
     */
    void handlePlaneSelection();

    /**
     * Handle profile drawing
     */
    void handleProfileDrawing();

    /**
     * Handle height adjustment
     */
    void handleHeightAdjustment();

    /**
     * Create the preview geometry
     */
    void createPreviewGeometry();

    /**
     * Update the preview geometry
     */
    void updatePreviewGeometry();

    /**
     * Finalize the primitive creation
     */
    void finalizePrimitive();

private:
    PrimitiveType _type;
    App::DocumentObject* _feature;
    DrawPrimitiveState _state;

    // Plane information
    Base::Vector3d _planeOrigin;
    Base::Vector3d _planeNormal;
    Base::Vector3d _planeU;  // First axis on the plane
    Base::Vector3d _planeV;  // Second axis on the plane

    // Profile points
    Base::Vector3d _profileStart;
    Base::Vector3d _profileEnd;
    bool _profileStarted;

    // Dimensions
    double _width;
    double _height;
    double _depth;

    // Preview
    std::unique_ptr<SoSeparator> _previewNode;
    SoCoordinate3* _previewCoords;
};

}  // namespace PartDesignGui

#endif  // PARTDESIGN_DrawPrimitiveHandler_H
