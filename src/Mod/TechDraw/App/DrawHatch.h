#ifndef TechDraw_DrawHatch_h_
#define TechDraw_DrawHatch_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyFile.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace App {
class Color;
}

namespace TechDraw
{
class DrawViewPart;

class TechDrawExport DrawHatch : public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawHatch);

public:
    DrawHatch();
    ~DrawHatch() override = default;

    App::PropertyLinkSub     Source;       // the dvp & face this hatch belongs to
    App::PropertyFile        HatchPattern;
    App::PropertyFileIncluded SvgIncluded;

    App::DocumentObjectExecReturn *execute() override;

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderHatch";
    }
    void setupObject() override;
    void unsetupObject() override;

    //return PyObject as DrawHatchPy
    PyObject *getPyObject() override;

    DrawViewPart* getSourceView() const;
    bool affectsFace(int i);
    bool removeSub(std::string toRemove);
    bool removeSub(int i);
    bool empty();
    static bool faceIsHatched(int i, std::vector<TechDraw::DrawHatch*> hatchObjs);
    static std::string prefSvgHatch();
    static Base::Color prefSvgHatchColor();

    bool isSvgHatch() const;
    bool isBitmapHatch() const;

    void translateLabel(std::string context, std::string baseName, std::string uniqueName);

protected:
    void onChanged(const App::Property* prop) override;
    void replaceFileIncluded(std::string newSvgFile);

private:

};

using DrawHatchPython = App::FeaturePythonT<DrawHatch>;

} //namespace TechDraw
#endif
