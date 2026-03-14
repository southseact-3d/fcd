#ifndef DRAWINGGUI_VIEWPROVIDERTEMPLATE_H
#define DRAWINGGUI_VIEWPROVIDERTEMPLATE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Gui/ViewProviderDocumentObject.h>

#include "ViewProviderTemplateExtension.h"
namespace TechDraw{
    class DrawTemplate;
}

namespace TechDrawGui {
class QGITemplate;
class MDIViewPage;

class TechDrawGuiExport ViewProviderTemplate : public Gui::ViewProviderDocumentObject,
                                               public ViewProviderTemplateExtension
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderTemplate);

public:
    /// constructor
    ViewProviderTemplate();
    /// destructor
    ~ViewProviderTemplate() override = default;

    void attach(App::DocumentObject *) override;

    bool useNewSelectionModel() const override {return false;}
    void updateData(const App::Property*) override;
    void onChanged(const App::Property *prop) override;
    void hide() override;
    void show() override;
    bool isShow() const override;
    QGITemplate* getQTemplate(void);
    TechDraw::DrawTemplate* getTemplate() const;
    MDIViewPage* getMDIViewPage(void) const;
    Gui::MDIView *getMDIView() const override;

    void setMarkers(bool state);
    bool onDelete(const std::vector<std::string> &) override;

    const char* whoAmI() const;

private:
    std::string m_myName;
};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERTEMPLATE_H

