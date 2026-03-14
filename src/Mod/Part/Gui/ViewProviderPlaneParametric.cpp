#include "ViewProviderPlaneParametric.h"
#include <Mod/Part/App/FeatureFace.h>
#include <Mod/Part/App/Part2DObject.h>


using namespace PartGui;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderPlaneParametric, PartGui::ViewProviderPrimitive)


ViewProviderPlaneParametric::ViewProviderPlaneParametric()
{
    sPixmap = "Part_Plane_Parametric";
}

ViewProviderPlaneParametric::~ViewProviderPlaneParametric() = default;

std::vector<std::string> ViewProviderPlaneParametric::getDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Flat Lines");
    StrList.emplace_back("Shaded");
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}

// ----------------------------------------------------------------------------

PROPERTY_SOURCE(PartGui::ViewProviderFace, PartGui::ViewProviderPlaneParametric)


ViewProviderFace::ViewProviderFace() = default;

ViewProviderFace::~ViewProviderFace() = default;

std::vector<App::DocumentObject*> ViewProviderFace::claimChildren() const
{
    return getObject<Part::Face>()->Sources.getValues();
}

bool ViewProviderFace::canDragObjects() const
{
    return true;
}

bool ViewProviderFace::canDragObject(App::DocumentObject* obj) const
{
    (void)obj;
    // return Part::Feature::hasShapeOwner(obj);
    return true;
}

void ViewProviderFace::dragObject(App::DocumentObject* obj)
{
    Part::Face* face = getObject<Part::Face>();
    std::vector<App::DocumentObject*> sources = face->Sources.getValues();
    for (std::vector<App::DocumentObject*>::iterator it = sources.begin(); it != sources.end(); ++it) {
        if (*it == obj) {
            sources.erase(it);
            face->Sources.setValues(sources);
            break;
        }
    }
}

bool ViewProviderFace::canDropObjects() const
{
    return true;
}

bool ViewProviderFace::canDropObject(App::DocumentObject* obj) const
{
    return canDragObject(obj);
}

void ViewProviderFace::dropObject(App::DocumentObject* obj)
{
    Part::Face* face = getObject<Part::Face>();
    std::vector<App::DocumentObject*> sources = face->Sources.getValues();
    sources.push_back(obj);
    face->Sources.setValues(sources);
}
