#ifndef VIEWPROVIDEDATUM_H_BYJRZNDL
#define VIEWPROVIDEDATUM_H_BYJRZNDL

#include "ViewProviderGeometryObject.h"

class SoScale;

namespace Gui
{

class SoShapeScale;

/**
 * View provider associated with an App::DatumElement.
 */
class GuiExport ViewProviderDatum: public ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderDatum);

public:
    ViewProviderDatum();
    ~ViewProviderDatum() override;

    /// Get point derived classes will add their specific stuff
    SoSeparator* getDatumRoot() const
    {
        return pRoot;
    }

    void attach(App::DocumentObject*) override;
    std::vector<std::string> getDisplayModes() const override;
    void setDisplayMode(const char* ModeName) override;

    /// @name Suppress ViewProviderGeometryObject's behaviour
    ///@{
    bool setEdit(int) override
    {
        return false;
    }
    void unsetEdit(int) override
    {}
    ///@}

    void setTemporaryScale(double factor);
    void resetTemporarySize();

protected:
    void onChanged(const App::Property* prop) override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
    SoSeparator* pRoot;
    SoShapeScale* soScale;
    double lineThickness;
};

}  // namespace Gui

#endif /* end of include guard: VIEWPROVIDEDATUM_H_BYJRZNDL */
