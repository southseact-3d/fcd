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

#include "DrawPrimitiveHandler.h"

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <App/Document.h>
#include <App/Origin.h>
#include <App/Plane.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/ViewProvider.h>
#include <Gui/ViewProviderCoordinateSystem.h>
#include <Gui/Selection.h>
#include <Gui/SelectionFilter.h>

#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/App/FeaturePrimitive.h>

using namespace PartDesignGui;

DrawPrimitiveHandler::DrawPrimitiveHandler(
    PrimitiveType type,
    App::DocumentObject* feature,
    QObject* parent)
    : QObject(parent)
    , Gui::SelectionObserver()
    , _type(type)
    , _feature(feature)
    , _state(DrawPrimitiveState::SelectPlane)
    , _planeOrigin(0, 0, 0)
    , _planeNormal(0, 0, 1)
    , _planeU(1, 0, 0)
    , _planeV(0, 1, 0)
    , _profileStart(0, 0, 0)
    , _profileEnd(0, 0, 0)
    , _profileStarted(false)
    , _width(10.0)
    , _height(10.0)
    , _depth(10.0)
    , _previewCoords(nullptr)
{
    // Initialize preview node
    _previewNode = std::make_unique<SoSeparator>();
    
    SoDrawStyle* drawStyle = new SoDrawStyle();
    drawStyle->lineWidth = 2.0f;
    _previewNode->addChild(drawStyle);
    
    _previewCoords = new SoCoordinate3();
    _previewNode->addChild(_previewCoords);
    
    SoLineSet* lineSet = new SoLineSet();
    lineSet->numPoints.setValue(0);
    _previewNode->addChild(lineSet);
}

DrawPrimitiveHandler::~DrawPrimitiveHandler()
{
    cancel();
}

void DrawPrimitiveHandler::start()
{
    Base::Console().log("DrawPrimitiveHandler: Starting interactive drawing\n");
    
    // Make origin planes visible for selection
    if (PartDesign::Body* body = PartDesign::Body::findBodyOf(_feature)) {
        try {
            App::Origin* origin = body->getOrigin();
            Gui::ViewProviderCoordinateSystem* vpOrigin = 
                static_cast<Gui::ViewProviderCoordinateSystem*>(
                    Gui::Application::Instance->getViewProvider(origin)
                );
            vpOrigin->setTemporaryVisibility(Gui::DatumElement::Planes | Gui::DatumElement::Axes);
        }
        catch (const Base::Exception& ex) {
            Base::Console().error("%s\n", ex.what());
        }
    }
    
    // Connect to selection changes
    Gui::Selection().addSelectionObserver(*this);
    
    _state = DrawPrimitiveState::SelectPlane;
    Base::Console().log("DrawPrimitiveHandler: Waiting for plane selection\n");
}

void DrawPrimitiveHandler::cancel()
{
    Base::Console().log("DrawPrimitiveHandler: Cancelled\n");
    
    // Disconnect from selection changes
    Gui::Selection().removeSelectionObserver(*this);
    
    // Hide origin planes
    if (PartDesign::Body* body = PartDesign::Body::findBodyOf(_feature)) {
        try {
            App::Origin* origin = body->getOrigin();
            Gui::ViewProviderCoordinateSystem* vpOrigin = 
                static_cast<Gui::ViewProviderCoordinateSystem*>(
                    Gui::Application::Instance->getViewProvider(origin)
                );
            vpOrigin->resetTemporaryVisibility();
        }
        catch (const Base::Exception& ex) {
            Base::Console().error("%s\n", ex.what());
        }
    }
    
    _state = DrawPrimitiveState::Finished;
    Q_EMIT cancelled();
}

void DrawPrimitiveHandler::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type != Gui::SelectionChanges::AddSelection) {
        return;
    }
    
    switch (_state) {
        case DrawPrimitiveState::SelectPlane:
            handlePlaneSelection();
            break;
        case DrawPrimitiveState::DrawProfile:
            handleProfileDrawing();
            break;
        case DrawPrimitiveState::AdjustHeight:
            handleHeightAdjustment();
            break;
        case DrawPrimitiveState::Finished:
            break;
    }
}

void DrawPrimitiveHandler::handlePlaneSelection()
{
    Base::Console().log("DrawPrimitiveHandler: Plane selected\n");
    
    // Get the selected object
    auto sel = Gui::Selection().getSelection();
    if (sel.empty()) {
        return;
    }
    
    App::DocumentObject* obj = sel[0].getObject();
    if (!obj) {
        return;
    }
    
    // Try to get plane information from the selected object
    // For now, we'll use a simple approach: check if it's an App::Plane
    if (obj->isDerivedFrom(App::Plane::getClassTypeId())) {
        App::Plane* plane = static_cast<App::Plane*>(obj);
        _planeOrigin = plane->Placement.getValue().getPosition();
        _planeNormal = plane->Placement.getValue().getRotation().multVec(Base::Vector3d(0, 0, 1));
    }
    else {
        // If it's a planar face, we need to get its normal
        // For now, use default XY plane
        _planeOrigin = Base::Vector3d(0, 0, 0);
        _planeNormal = Base::Vector3d(0, 0, 1);
    }
    
    // Calculate plane axes
    if (std::abs(_planeNormal.z) > 0.9) {
        _planeU = Base::Vector3d(1, 0, 0);
        _planeV = Base::Vector3d(0, 1, 0);
    }
    else if (std::abs(_planeNormal.x) > 0.9) {
        _planeU = Base::Vector3d(0, 1, 0);
        _planeV = Base::Vector3d(0, 0, 1);
    }
    else {
        _planeU = Base::Vector3d(1, 0, 0);
        _planeV = Base::Vector3d(0, 0, 1);
    }
    
    _state = DrawPrimitiveState::DrawProfile;
    _profileStarted = false;
    
    Q_EMIT planeSelected(_planeOrigin, _planeNormal);
    Base::Console().log("DrawPrimitiveHandler: Ready for profile drawing\n");
}

void DrawPrimitiveHandler::handleProfileDrawing()
{
    if (!_profileStarted) {
        // First click: start point
        auto sel = Gui::Selection().getSelection();
        if (!sel.empty()) {
            _profileStart = sel[0].getPoint();
            _profileStarted = true;
            Base::Console().log("DrawPrimitiveHandler: Profile start point set\n");
        }
    }
    else {
        // Second click: end point
        auto sel = Gui::Selection().getSelection();
        if (!sel.empty()) {
            _profileEnd = sel[0].getPoint();
            
            // Calculate dimensions
            if (_type == PrimitiveType::Box) {
                _width = std::abs(_profileEnd.x - _profileStart.x);
                _height = std::abs(_profileEnd.y - _profileStart.y);
                _depth = 10.0;  // Default depth
            }
            else if (_type == PrimitiveType::Cylinder) {
                Base::Vector3d diff = _profileEnd - _profileStart;
                _width = diff.Length();  // Diameter
                _height = 10.0;  // Default height
            }
            
            _profileStarted = false;
            _state = DrawPrimitiveState::AdjustHeight;
            
            Q_EMIT profileDrawn(_width, _height);
            Base::Console().log("DrawPrimitiveHandler: Profile drawn, adjusting height\n");
        }
    }
}

void DrawPrimitiveHandler::handleHeightAdjustment()
{
    // For now, just finalize with default height
    // In a full implementation, this would handle mouse drag for height adjustment
    finalizePrimitive();
}

void DrawPrimitiveHandler::createPreviewGeometry()
{
    // Clear existing preview
    _previewCoords->point.deleteValues(0);
    
    if (_type == PrimitiveType::Box) {
        // Create rectangle preview
        SbVec3f points[5];
        points[0] = SbVec3f(_profileStart.x, _profileStart.y, _profileStart.z);
        points[1] = SbVec3f(_profileEnd.x, _profileStart.y, _profileStart.z);
        points[2] = SbVec3f(_profileEnd.x, _profileEnd.y, _profileStart.z);
        points[3] = SbVec3f(_profileStart.x, _profileEnd.y, _profileStart.z);
        points[4] = points[0];
        
        _previewCoords->point.setValues(0, 5, points);
    }
    else if (_type == PrimitiveType::Cylinder) {
        // Create circle preview (simplified as polygon)
        int numSegments = 36;
        SbVec3f* points = new SbVec3f[numSegments + 1];
        
        double radius = _width / 2.0;
        for (int i = 0; i <= numSegments; ++i) {
            double angle = 2.0 * M_PI * i / numSegments;
            points[i] = SbVec3f(
                _profileStart.x + radius * std::cos(angle),
                _profileStart.y + radius * std::sin(angle),
                _profileStart.z
            );
        }
        
        _previewCoords->point.setValues(0, numSegments + 1, points);
        delete[] points;
    }
}

void DrawPrimitiveHandler::updatePreviewGeometry()
{
    createPreviewGeometry();
}

void DrawPrimitiveHandler::finalizePrimitive()
{
    Base::Console().log("DrawPrimitiveHandler: Finalizing primitive creation\n");
    
    // Disconnect from selection changes
    Gui::Selection().removeSelectionObserver(*this);
    
    // Hide origin planes
    if (PartDesign::Body* body = PartDesign::Body::findBodyOf(_feature)) {
        try {
            App::Origin* origin = body->getOrigin();
            Gui::ViewProviderCoordinateSystem* vpOrigin = 
                static_cast<Gui::ViewProviderCoordinateSystem*>(
                    Gui::Application::Instance->getViewProvider(origin)
                );
            vpOrigin->resetTemporaryVisibility();
        }
        catch (const Base::Exception& ex) {
            Base::Console().error("%s\n", ex.what());
        }
    }
    
    // Set the feature dimensions
    if (_type == PrimitiveType::Box) {
        if (auto* box = dynamic_cast<PartDesign::Box*>(_feature)) {
            box->Length.setValue(_width);
            box->Width.setValue(_height);
            box->Height.setValue(_depth);
        }
    }
    else if (_type == PrimitiveType::Cylinder) {
        if (auto* cyl = dynamic_cast<PartDesign::Cylinder*>(_feature)) {
            cyl->Radius.setValue(_width / 2.0);
            cyl->Height.setValue(_height);
        }
    }
    
    _state = DrawPrimitiveState::Finished;
    Q_EMIT finished();
    Base::Console().log("DrawPrimitiveHandler: Primitive creation complete\n");
}
