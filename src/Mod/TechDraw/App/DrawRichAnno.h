#ifndef TechDraw_DrawRichAnno_h_
#define TechDraw_DrawRichAnno_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawView.h"


namespace TechDraw
{

class TechDrawExport DrawRichAnno : public TechDraw::DrawView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawRichAnno);

public:
    DrawRichAnno();
    ~DrawRichAnno() override = default;

    App::PropertyLink         AnnoParent;
    App::PropertyString       AnnoText;
    App::PropertyBool         ShowFrame;
    App::PropertyFloat        MaxWidth;

    short mustExecute() const override;
    App::DocumentObjectExecReturn *execute() override;

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderRichAnno";
    }
    PyObject *getPyObject() override;
    QRectF getRect() const override { return { 0, 0, 1, 1}; }
    DrawView* getBaseView() const;

    DrawPage* findParentPage() const override;
    App::PropertyLink *getOwnerProperty() override { return &AnnoParent; }

protected:
    void onChanged(const App::Property* prop) override;

private:
};

using DrawRichAnnoPython = App::FeaturePythonT<DrawRichAnno>;

} //namespace TechDraw
#endif
