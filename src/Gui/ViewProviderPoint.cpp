#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>


#include "ViewProviderPoint.h"
#include "ViewProviderCoordinateSystem.h"

using namespace Gui;

PROPERTY_SOURCE(Gui::ViewProviderPoint, Gui::ViewProviderDatum)

ViewProviderPoint::ViewProviderPoint()
{
    sPixmap = "Std_Point";  // Set the icon for the point
}

ViewProviderPoint::~ViewProviderPoint() = default;

void ViewProviderPoint::attach(App::DocumentObject* obj)
{
    ViewProviderDatum::attach(obj);

    // The coordinates for the point (single vertex at the origin)
    static const SbVec3f point = SbVec3f(0, 0, 0);

    SoSeparator* sep = getDatumRoot();

    auto pCoords = new SoCoordinate3();
    pCoords->point.setNum(1);
    pCoords->point.setValue(point);
    sep->addChild(pCoords);

    static const float size = App::GetApplication()
                                  .GetParameterGroupByPath("User parameter:BaseApp/Preferences/View")
                                  ->GetFloat("DatumPointSize", 2.5);
    auto sphere = new SoSphere();
    sphere->radius.setValue(size);
    sep->addChild(sphere);

    // Add pick style to define how the point can be selected
    auto ps = new SoPickStyle();
    ps->style.setValue(SoPickStyle::BOUNDING_BOX);
    sep->addChild(ps);
}
