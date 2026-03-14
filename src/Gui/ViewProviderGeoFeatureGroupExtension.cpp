#include <Inventor/nodes/SoSeparator.h>


#include <App/DocumentObject.h>
#include <App/GeoFeatureGroupExtension.h>

#include "ViewProviderGeoFeatureGroupExtension.h"
#include "ViewProviderDocumentObject.h"
#include "Application.h"
#include "SoFCUnifiedSelection.h"


using namespace Gui;

EXTENSION_PROPERTY_SOURCE(Gui::ViewProviderGeoFeatureGroupExtension, Gui::ViewProviderGroupExtension)

ViewProviderGeoFeatureGroupExtension::ViewProviderGeoFeatureGroupExtension()
{
    initExtensionType(ViewProviderGeoFeatureGroupExtension::getExtensionClassTypeId());

    pcGroupChildren = new SoFCSelectionRoot;
    pcGroupChildren->ref();
    pcGroupFront = new SoSeparator();
    pcGroupFront->ref();
    pcGroupBack = new SoSeparator();
    pcGroupBack->ref();
}

ViewProviderGeoFeatureGroupExtension::~ViewProviderGeoFeatureGroupExtension()
{
    pcGroupChildren->unref();
    pcGroupChildren = nullptr;
    pcGroupFront->unref();
    pcGroupFront = nullptr;
    pcGroupBack->unref();
    pcGroupBack = nullptr;
}


std::vector<App::DocumentObject*> ViewProviderGeoFeatureGroupExtension::extensionClaimChildren3D() const
{

    // all object in the group must be claimed in 3D, as we are a coordinate system for all of them
    auto* obj = getExtendedViewProvider()->getObject();
    auto* ext = obj ? obj->getExtensionByType<App::GeoFeatureGroupExtension>() : nullptr;
    if (ext) {
        auto objs = ext->Group.getValues();
        return objs;
    }
    return {};
}

std::vector<App::DocumentObject*> ViewProviderGeoFeatureGroupExtension::extensionClaimChildren() const
{

    auto* obj = getExtendedViewProvider()->getObject();
    if (!obj) {
        return {};
    }

    auto* group = obj->getExtensionByType<App::GeoFeatureGroupExtension>();
    const std::vector<App::DocumentObject*>& model = group->Group.getValues();
    std::set<App::DocumentObject*> outSet;  //< set of objects not to claim (childrens of childrens)

    // search for objects handled (claimed) by the features
    for (auto obj : model) {
        // stuff in another geofeaturegroup is not in the model anyway
        if (!obj || obj->hasExtension(App::GeoFeatureGroupExtension::getExtensionClassTypeId())) {
            continue;
        }

        Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(obj);
        if (!vp || vp == getExtendedViewProvider()) {
            continue;
        }

        auto children = vp->claimChildren();
        std::remove_copy(children.begin(), children.end(), std::inserter(outSet, outSet.begin()), nullptr);
    }

    // remove the otherwise handled objects, preserving their order so the order in the TreeWidget
    // is correct
    std::vector<App::DocumentObject*> Result;
    for (auto obj : model) {
        if (!obj || !obj->isAttachedToDocument()) {
            continue;
        }
        if (outSet.contains(obj)) {
            obj->setStatus(App::ObjectStatus::GeoExcluded, true);
        }
        else {
            obj->setStatus(App::ObjectStatus::GeoExcluded, false);
            Result.push_back(obj);
        }
    }
    return Result;
}

void ViewProviderGeoFeatureGroupExtension::extensionFinishRestoring()
{
    // setup GeoExlcuded flag for children
    extensionClaimChildren();
    ViewProviderGroupExtension::extensionFinishRestoring();
}

void ViewProviderGeoFeatureGroupExtension::extensionAttach(App::DocumentObject* pcObject)
{
    ViewProviderGroupExtension::extensionAttach(pcObject);
    getExtendedViewProvider()->addDisplayMaskMode(pcGroupChildren, "Group");
}

void ViewProviderGeoFeatureGroupExtension::extensionSetDisplayMode(const char* ModeName)
{
    if (strcmp("Group", ModeName) == 0) {
        getExtendedViewProvider()->setDisplayMaskMode("Group");
    }

    ViewProviderGroupExtension::extensionSetDisplayMode(ModeName);
}

std::vector<std::string> ViewProviderGeoFeatureGroupExtension::extensionGetDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList = ViewProviderGroupExtension::extensionGetDisplayModes();

    // add your own modes
    StrList.emplace_back("Group");

    return StrList;
}

void ViewProviderGeoFeatureGroupExtension::extensionUpdateData(const App::Property* prop)
{
    auto obj = getExtendedViewProvider()->getObject();
    auto grp = obj ? obj->getExtensionByType<App::GeoFeatureGroupExtension>() : nullptr;
    if (grp && prop == &grp->placement()) {
        getExtendedViewProvider()->setTransformation(grp->placement().getValue().toMatrix());
    }
    else {
        ViewProviderGroupExtension::extensionUpdateData(prop);
    }
}

namespace Gui
{
EXTENSION_PROPERTY_SOURCE_TEMPLATE(
    Gui::ViewProviderGeoFeatureGroupExtensionPython,
    Gui::ViewProviderGeoFeatureGroupExtension
)

// explicit template instantiation
template class GuiExport ViewProviderExtensionPythonT<ViewProviderGeoFeatureGroupExtension>;
}  // namespace Gui
