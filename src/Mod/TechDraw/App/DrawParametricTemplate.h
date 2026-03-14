#ifndef TECHDRAW_DrawParametricTemplate_h_
#define TECHDRAW_DrawParametricTemplate_h_

#include <App/FeaturePython.h>
#include <App/PropertyFile.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawTemplate.h"
#include "Geometry.h"


namespace TechDraw
{
    class BaseGeom;
}

//TODO: DrawParametricTemplate class is obsolete

namespace TechDraw
{

class TechDrawExport DrawParametricTemplate: public TechDraw::DrawTemplate
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawParametricTemplate);

public:
    DrawParametricTemplate(); /// Constructor
    ~DrawParametricTemplate() override;

    App::PropertyFile Template;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}


    short mustExecute() const override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderTemplate";
    }

    // from base class
    PyObject *getPyObject() override;
    unsigned int getMemSize() const override;

public:
    std::vector<TechDraw::BaseGeomPtr> getGeometry() { return geom; }
    int clearGeometry();

    // Template Drawing Methods
    int drawLine(double x1, double y1, double x2, double y2);

    double getHeight() const override;
    double getWidth() const override;

protected:
    void onChanged(const App::Property* prop) override;

protected:
    std::vector<TechDraw::BaseGeomPtr> geom;

private:
    static const char* OrientationEnums[];
};

using DrawParametricTemplatePython = App::FeaturePythonT<DrawParametricTemplate>;

} //namespace TechDraw



#endif //_TECHDRAW_DrawParametricTemplate_h_
