#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoVertexProperty.h>


#include <App/Application.h>
#include <Gui/Inventor/MarkerBitmaps.h>
#include <Gui/ViewProviderBuilder.h>
#include <Gui/Selection/SoFCSelection.h>
#include <Mod/PartDesign/App/DatumPoint.h>

#include "ViewProviderDatumPoint.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderDatumPoint, PartDesignGui::ViewProviderDatum)

ViewProviderDatumPoint::ViewProviderDatumPoint()
{
    sPixmap = "PartDesign_Point.svg";

    // SoMarkerSet won't be drawn if transparency is nonzero, so disabble it
    Transparency.setValue(0);
    Transparency.setStatus(App::Property::Hidden, true);  //< make transparency hidden
}

ViewProviderDatumPoint::~ViewProviderDatumPoint() = default;

void ViewProviderDatumPoint::attach(App::DocumentObject* obj)
{
    ViewProviderDatum::attach(obj);

    const int markerSize = App::GetApplication()
                               .GetParameterGroupByPath("User parameter:BaseApp/Preferences/View")
                               ->GetInt("MarkerSize", 9);
    // Using a marker gives a larger point.
    auto* marker = new SoMarkerSet();
    marker->numPoints = 1;
    marker->markerIndex = Gui::Inventor::MarkerBitmaps::getMarkerIndex("DIAMOND_FILLED", markerSize);

    auto* pcHighlight = Gui::ViewProviderBuilder::createSelection();
    pcHighlight->style = Gui::SoFCSelection::EMISSIVE_DIFFUSE;
    pcHighlight->addChild(new SoCoordinate3());
    pcHighlight->addChild(marker);

    getShapeRoot()->addChild(pcHighlight);
}

void ViewProviderDatumPoint::onChanged(const App::Property* prop)
{
    // Forbid to set trancparency
    if (prop == &Transparency && Transparency.getValue() != 0) {
        Transparency.setValue(0);
    }

    ViewProviderDatum::onChanged(prop);
}
