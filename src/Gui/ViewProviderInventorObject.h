#ifndef GUI_VIEWPROVIDER_INVENTOROBJECT_H
#define GUI_VIEWPROVIDER_INVENTOROBJECT_H

#include "ViewProviderDocumentObject.h"


namespace Gui
{

class GuiExport ViewProviderInventorObject: public ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderInventorObject);

public:
    /// constructor.
    ViewProviderInventorObject();

    /// destructor.
    ~ViewProviderInventorObject() override;

    void attach(App::DocumentObject* pcObject) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override;
    bool useNewSelectionModel() const override
    {
        return true;
    }

private:
    void adjustSelectionNodes(SoNode* child, const char* docname, const char* objname);

protected:
    SoSeparator* pcBuffer;
    SoSeparator* pcFile;
};

}  // namespace Gui


#endif  // GUI_VIEWPROVIDER_INVENTOROBJECT_H
