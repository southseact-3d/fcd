#include <Gui/Application.h>
#include <Mod/Part/App/PartFeatures.h>

#include "ViewProviderRuledSurface.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderRuledSurface, PartGui::ViewProviderPart)


ViewProviderRuledSurface::ViewProviderRuledSurface()
{
    sPixmap = "Part_RuledSurface.svg";
}

ViewProviderRuledSurface::~ViewProviderRuledSurface() = default;

std::vector<App::DocumentObject*> ViewProviderRuledSurface::claimChildren() const
{
    // in a set each element is unique
    std::set<App::DocumentObject*> temp;
    temp.insert(getObject<Part::RuledSurface>()->Curve1.getValue());
    temp.insert(getObject<Part::RuledSurface>()->Curve2.getValue());

    std::vector<App::DocumentObject*> array;
    array.insert(array.begin(), temp.begin(), temp.end());
    return array;
}

void ViewProviderRuledSurface::updateData(const App::Property* prop)
{
    PartGui::ViewProviderPart::updateData(prop);
    if (prop->is<Part::PropertyShapeHistory>()) {
        // const std::vector<Part::ShapeHistory>& hist = static_cast<const
        // Part::PropertyShapeHistory*>
        //     (prop)->getValues();
    }
    /*  //The following hides the Children shapes. If the edges from which the Ruled Surface was
    created
     * were selected from the subshapes of another shape, it is likely that one would not want to
    hide the shape
     * hence this section is commented out
    Part::RuledSurface* pRuledSurface = getObject<Part::RuledSurface>();
    App::DocumentObject *pCurve1 = pRuledSurface->Curve1.getValue();
    App::DocumentObject *pCurve2 = pRuledSurface->Curve2.getValue();
    if (pCurve1)
        Gui::Application::Instance->hideViewProvider(pCurve1);
    if (pCurve2)
        Gui::Application::Instance->hideViewProvider(pCurve2);*/
}

bool ViewProviderRuledSurface::onDelete(const std::vector<std::string>&)
{
    // get the input shape
    Part::RuledSurface* pRuledSurface = getObject<Part::RuledSurface>();
    App::DocumentObject* pCurve1 = pRuledSurface->Curve1.getValue();
    App::DocumentObject* pCurve2 = pRuledSurface->Curve2.getValue();
    if (pCurve1) {
        Gui::Application::Instance->showViewProvider(pCurve1);
    }
    if (pCurve2) {
        Gui::Application::Instance->showViewProvider(pCurve2);
    }

    return true;
}


// **********************************************************************************

std::vector<std::string> ViewProviderRuledSurface::getDisplayModes() const
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
