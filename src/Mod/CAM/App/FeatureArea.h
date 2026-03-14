#ifndef PATH_FeatureArea_H
#define PATH_FeatureArea_H

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyUnits.h>
#include "Mod/Part/App/PartFeature.h"

#include "Area.h"


namespace Path
{

class PathExport FeatureArea: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Path::FeatureArea);

public:
    /// Constructor
    FeatureArea();
    ~FeatureArea() override;

    Area& getArea();
    const std::vector<TopoDS_Shape>& getShapes();

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PathGui::ViewProviderArea";
    }
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    PyObject* getPyObject() override;

    App::PropertyLinkList Sources;
    Part::PropertyPartShape WorkPlane;

    PARAM_PROP_DECLARE(AREA_PARAMS_ALL)

    void setWorkPlane(const TopoDS_Shape& shape)
    {
        WorkPlane.setValue(shape);
        myArea.setPlane(shape);
    }

private:
    Area myArea;
    std::vector<TopoDS_Shape> myShapes;
    bool myInited;
};

using FeatureAreaPython = App::FeaturePythonT<FeatureArea>;

class PathExport FeatureAreaView: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Path::FeatureAreaView);

public:
    /// Constructor
    FeatureAreaView();

    std::list<TopoDS_Shape> getShapes();

    const char* getViewProviderName() const override
    {
        return "PathGui::ViewProviderAreaView";
    }
    App::DocumentObjectExecReturn* execute() override;

    App::PropertyLink Source;
    App::PropertyInteger SectionIndex;
    App::PropertyInteger SectionCount;
};

using FeatureAreaViewPython = App::FeaturePythonT<FeatureAreaView>;

}  // namespace Path


#endif  // PATH_FeaturePath_H
