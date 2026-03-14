#ifndef TechDraw_DrawTile_h_
#define TechDraw_DrawTile_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"


namespace TechDraw
{

class TechDrawExport DrawTile : public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawTile);

public:
    DrawTile();
    ~DrawTile() override = default;

    App::PropertyLink         TileParent;           //eg DrawWeldSymbol
    App::PropertyIntegerConstraint TileRow;
    App::PropertyIntegerConstraint::Constraints  TileRowConstraints;
    App::PropertyInteger      TileColumn;

    short mustExecute() const override;
    App::DocumentObjectExecReturn *execute(void) override;

    const char* getViewProviderName(void) const override {
        return "TechDrawGui::ViewProviderTile";
    }
    PyObject *getPyObject(void) override;
    virtual DrawView* getParent(void) const;

protected:
    void onChanged(const App::Property* prop) override;
    void handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property * prop) override;

private:
};

using DrawTilePython = App::FeaturePythonT<DrawTile>;

} //namespace TechDraw
#endif
