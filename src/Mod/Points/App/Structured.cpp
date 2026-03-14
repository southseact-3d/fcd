#include <vector>


#include "Structured.h"


using namespace Points;


//===========================================================================
// Structured
//===========================================================================
/*
import Points
import random
import math

r=random.Random()

p=Points.Points()
pts=[]
for i in range(21):
  for j in range(21):
    pts.append(App.Vector(i,j,r.gauss(5,0.05)))

p.addPoints(pts)
doc=App.ActiveDocument
pts=doc.addObject('Points::Structured','View')
pts.Points=p
pts.Width=21
pts.Height=21
*/

// ---------------------------------------------------------

PROPERTY_SOURCE(Points::Structured, Points::Feature)

Structured::Structured()
{
    //    App::PropertyType type = static_cast<App::PropertyType>(App::Prop_None);
    App::PropertyType type = static_cast<App::PropertyType>(App::Prop_ReadOnly);
    ADD_PROPERTY_TYPE(Width, (1), "Structured points", type, "Width of the image");
    ADD_PROPERTY_TYPE(Height, (1), "Structured points", type, "Height of the image");
    // Width.setStatus(App::Property::ReadOnly, true);
    // Height.setStatus(App::Property::ReadOnly, true);
}

App::DocumentObjectExecReturn* Structured::execute()
{
    std::size_t size = Height.getValue() * Width.getValue();
    if (size != Points.getValue().size()) {
        throw Base::ValueError("(Width * Height) doesn't match with number of points");
    }
    this->Points.touch();
    return App::DocumentObject::StdReturn;
}

// ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Points::StructuredCustom, Points::Structured)
/// @endcond

// explicit template instantiation
template class PointsExport FeatureCustomT<Points::Structured>;
}  // namespace App
