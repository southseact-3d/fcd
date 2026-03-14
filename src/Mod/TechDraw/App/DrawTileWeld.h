#ifndef TechDraw_DrawTileWeld_h_
#define TechDraw_DrawTileWeld_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyFile.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawTile.h"


namespace TechDraw
{

class TechDrawExport DrawTileWeld : public TechDraw::DrawTile
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawTileWeld);

public:
    DrawTileWeld();
    ~DrawTileWeld() override;

    App::PropertyString       LeftText;
    App::PropertyString       RightText;
    App::PropertyString       CenterText;
    App::PropertyFile         SymbolFile;
    App::PropertyFileIncluded SymbolIncluded;

    void setupObject() override;

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderTile";
    }
    PyObject *getPyObject() override;
    virtual QRectF getRect() const { return { 0, 0, 1, 1}; }

    void replaceFileIncluded(std::string newSymbolFile);

    std::string prefSymbol();

protected:
    void onChanged(const App::Property* prop) override;

private:
};

using DrawTileWeldPython = App::FeaturePythonT<DrawTileWeld>;

} //namespace TechDraw
#endif
