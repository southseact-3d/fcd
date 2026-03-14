#ifndef GUI_VIEWPROVIDER_EXTERN_H
#define GUI_VIEWPROVIDER_EXTERN_H

#include "ViewProvider.h"


class SoNode;
class SoInput;

namespace Gui
{


class GuiExport ViewProviderExtern: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderExtern);

public:
    /// constructor.
    ViewProviderExtern();

    /// destructor.
    ~ViewProviderExtern() override;

    void setModeByString(const char* name, const char* ivFragment);
    void setModeByFile(const char* name, const char* ivFileName);
    void setModeBySoInput(const char* name, SoInput& ivFileInput);
    void adjustDocumentName(const char* docname);

    const char* getDefaultDisplayMode() const override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override
    {}

private:
    void adjustRecursiveDocumentName(SoNode*, const char* docname);

protected:
    std::vector<std::string> modes;
};

}  // namespace Gui

#endif  // GUI_VIEWPROVIDER_EXTERN_H
