#include <App/DocumentObject.h>
#include "ViewProviderTile.h"

using namespace TechDrawGui;

PROPERTY_SOURCE(TechDrawGui::ViewProviderTile, Gui::ViewProviderDocumentObject)

//**************************************************************************
// Construction/Destruction

ViewProviderTile::ViewProviderTile()
{
    sPixmap = "actions/TechDraw_Tile";
}

ViewProviderTile::~ViewProviderTile()
{
}

//TechDraw::DrawTile* ViewProviderTile::getViewObject() const
//{
//    return dynamic_cast<TechDraw::DrawTile*>(pcObject);
//}

bool ViewProviderTile::canDelete(App::DocumentObject *obj) const
{
    // deletions of Tile objects don't destroy anything
    // thus we can pass this action
    // that the deletion of its parent weld symbol is not possible
    // is done in its onDelete() function
    Q_UNUSED(obj)
    return true;
}

TechDraw::DrawTile* ViewProviderTile::getFeature() const
{
    return dynamic_cast<TechDraw::DrawTile*>(pcObject);
}
