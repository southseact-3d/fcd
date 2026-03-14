#ifndef TechDraw_DrawWeldSymbol_h_
#define TechDraw_DrawWeldSymbol_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"


namespace TechDraw
{
class DrawTile;
class DrawTileWeld;

class TechDrawExport DrawWeldSymbol : public TechDraw::DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawWeldSymbol);

public:
    DrawWeldSymbol();
    ~DrawWeldSymbol() override = default;

    App::PropertyLink         Leader;
    App::PropertyBool         AllAround;
    App::PropertyBool         FieldWeld;
    App::PropertyBool         AlternatingWeld;
    App::PropertyString       TailText;

    short mustExecute() const override;
    App::DocumentObjectExecReturn *execute() override;
    void onSettingDocument() override;

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderWeld";
    }
    PyObject *getPyObject() override;
    QRectF getRect() const override { return { 0, 0, 1, 1}; }

    bool isTailRightSide();
    std::vector<DrawTileWeld*> getTiles() const;

    App::PropertyLink *getOwnerProperty() override { return &Leader; }

protected:
    void onChanged(const App::Property* prop) override;

private:
};

using DrawWeldSymbolPython = App::FeaturePythonT<DrawWeldSymbol>;

} //namespace TechDraw
#endif
