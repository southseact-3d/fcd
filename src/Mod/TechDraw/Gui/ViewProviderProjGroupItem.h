#ifndef DRAWINGGUI_VIEWPROVIDERVIEWGROUPITEM_H
#define DRAWINGGUI_VIEWPROVIDERVIEWGROUPITEM_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawProjGroupItem.h>

#include "ViewProviderViewPart.h"


namespace TechDrawGui {


class TechDrawGuiExport ViewProviderProjGroupItem: public ViewProviderViewPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderProjGroupItem);

public:

     ViewProviderProjGroupItem();
     ~ViewProviderProjGroupItem() override;

    bool useNewSelectionModel() const override {return false;}

    /// Is called by the tree if the user double click on the object
    bool doubleClicked() override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    void updateData(const App::Property*) override;
    void updateIcon();

    TechDraw::DrawProjGroupItem* getViewObject() const override;
    TechDraw::DrawProjGroupItem* getObject() const;
    void unsetEdit(int ModNum) override;
    bool onDelete(const std::vector<std::string> &) override;
    bool canDelete(App::DocumentObject* obj) const override;

protected:
    bool setEdit(int ModNum) override;

};

} // namespace TechDrawGui

#endif // DRAWINGGUI_VIEWPROVIDERVIEWGROUPITEM_H
