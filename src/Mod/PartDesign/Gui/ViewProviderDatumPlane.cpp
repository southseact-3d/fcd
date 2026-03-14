#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>


#include <Mod/Part/Gui/SoBrepEdgeSet.h>
#include <Mod/Part/Gui/SoBrepFaceSet.h>
#include <Mod/PartDesign/App/DatumPlane.h>

#include "ViewProviderDatumPlane.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderDatumPlane, PartDesignGui::ViewProviderDatum)

ViewProviderDatumPlane::ViewProviderDatumPlane()
{
    sPixmap = "PartDesign_Plane.svg";

    pCoords = new SoCoordinate3();
    pCoords->ref();
}

ViewProviderDatumPlane::~ViewProviderDatumPlane()
{
    pCoords->unref();
}

void ViewProviderDatumPlane::attach(App::DocumentObject* obj)
{

    ViewProviderDatum::attach(obj);

    ViewProviderDatum::setExtents(defaultBoundBox());
    getShapeRoot()->addChild(pCoords);

    PartGui::SoBrepEdgeSet* lineSet = new PartGui::SoBrepEdgeSet();
    lineSet->coordIndex.setNum(6);
    lineSet->coordIndex.set1Value(0, 0);
    lineSet->coordIndex.set1Value(1, 1);
    lineSet->coordIndex.set1Value(2, 2);
    lineSet->coordIndex.set1Value(3, 3);
    lineSet->coordIndex.set1Value(4, 0);
    lineSet->coordIndex.set1Value(5, SO_END_LINE_INDEX);
    getShapeRoot()->addChild(lineSet);

    PartGui::SoBrepFaceSet* faceSet = new PartGui::SoBrepFaceSet();
    // SoBrepFaceSet supports only triangles (otherwise we receive incorrect highlighting)
    faceSet->partIndex.set1Value(0, 2);  // One face, two triangles
    faceSet->coordIndex.setNum(8);
    // first triangle
    faceSet->coordIndex.set1Value(0, 0);
    faceSet->coordIndex.set1Value(1, 1);
    faceSet->coordIndex.set1Value(2, 2);
    faceSet->coordIndex.set1Value(3, SO_END_FACE_INDEX);
    // second triangle
    faceSet->coordIndex.set1Value(4, 2);
    faceSet->coordIndex.set1Value(5, 3);
    faceSet->coordIndex.set1Value(6, 0);
    faceSet->coordIndex.set1Value(7, SO_END_FACE_INDEX);

    getShapeRoot()->addChild(faceSet);
}

void ViewProviderDatumPlane::updateData(const App::Property* prop)
{
    // Gets called whenever a property of the attached object changes
    if (strcmp(prop->getName(), "Placement") == 0) {
        updateExtents();
    }
    else if (strcmp(prop->getName(), "Length") == 0 || strcmp(prop->getName(), "Width") == 0) {
        PartDesign::Plane* pcDatum = this->getObject<PartDesign::Plane>();
        if (pcDatum->ResizeMode.getValue() != 0) {
            setExtents(pcDatum->Length.getValue(), pcDatum->Width.getValue());
        }
    }

    ViewProviderDatum::updateData(prop);
}


void ViewProviderDatumPlane::setExtents(Base::BoundBox3d bbox)
{
    PartDesign::Plane* pcDatum = this->getObject<PartDesign::Plane>();
    if (pcDatum->ResizeMode.getValue() != 0) {
        setExtents(pcDatum->Length.getValue(), pcDatum->Width.getValue());
        return;
    }

    Base::Placement plm = pcDatum->Placement.getValue().inverse();

    // Transform the box to the line's coordinates, the result line will be larger than the bbox
    bbox = bbox.Transformed(plm.toMatrix());
    // Add origin of the plane to the box if it's not
    bbox.Add(Base::Vector3d(0, 0, 0));

    double margin = sqrt(bbox.LengthX() * bbox.LengthY()) * marginFactor();

    pcDatum->Length.setValue(bbox.LengthX() + 2 * margin);
    pcDatum->Width.setValue(bbox.LengthY() + 2 * margin);

    // Change the coordinates of the line
    pCoords->point.setNum(4);
    pCoords->point.set1Value(0, bbox.MaxX + margin, bbox.MaxY + margin, 0);
    pCoords->point.set1Value(1, bbox.MinX - margin, bbox.MaxY + margin, 0);
    pCoords->point.set1Value(2, bbox.MinX - margin, bbox.MinY - margin, 0);
    pCoords->point.set1Value(3, bbox.MaxX + margin, bbox.MinY - margin, 0);
}

void ViewProviderDatumPlane::setExtents(double l, double w)
{
    // Change the coordinates of the line
    pCoords->point.setNum(4);
    pCoords->point.set1Value(0, l / 2, w / 2, 0);
    pCoords->point.set1Value(1, -l / 2, w / 2, 0);
    pCoords->point.set1Value(2, -l / 2, -w / 2, 0);
    pCoords->point.set1Value(3, l / 2, -w / 2, 0);
}
