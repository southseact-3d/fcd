#ifndef GUI_VIEWPROVIDER_VRMLOROBJECT_H
#define GUI_VIEWPROVIDER_VRMLOROBJECT_H

#include "ViewProviderDocumentObject.h"

class SbString;

namespace Gui
{

class SoFCSelection;
class GuiExport ViewProviderVRMLObject: public ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderVRMLObject);

public:
    /// constructor.
    ViewProviderVRMLObject();

    /// destructor.
    ~ViewProviderVRMLObject() override;

    void attach(App::DocumentObject* pcObject) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override;
    void getLocalResources(SoNode*, std::list<std::string>&);
    void addResource(const SbString&, std::list<std::string>&);
    template<typename T>
    void getResourceFile(SoNode*, std::list<std::string>&);

protected:
    SoFCSelection* pcVRML;
};

}  // namespace Gui


#endif  // GUI_VIEWPROVIDER_INVENTOROBJECT_H
