#ifndef Fem_FemResultObject_H
#define Fem_FemResultObject_H

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <Mod/Fem/FemGlobal.h>


namespace Fem
{
/// Father of all result data in a Fem Analysis
class FemExport FemResultObject: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemResultObject);

public:
    /// Constructor
    FemResultObject();
    ~FemResultObject() override;

    App::PropertyIntegerList NodeNumbers;
    /// Link to the corresponding mesh
    App::PropertyLink Mesh;
    /// Stats of analysis
    App::PropertyFloat Time;
    /// User defined results
    App::PropertyFloatList Stats;
    /// Displacement vectors of analysis

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderResult";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

using FemResultObjectPython = App::FeaturePythonT<FemResultObject>;


}  // namespace Fem


#endif  // Fem_FemResultObject_H
