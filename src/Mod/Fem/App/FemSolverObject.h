#ifndef Fem_FemSolverObject_H
#define Fem_FemSolverObject_H

#include <App/FeaturePython.h>
#include <App/PropertyFile.h>
#include <Mod/Fem/FemGlobal.h>

namespace Fem
{
/// Father of all result data in a Fem Analysis
class FemExport FemSolverObject: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSolverObject);

public:
    /// Constructor
    FemSolverObject();
    ~FemSolverObject() override;

    App::PropertyLinkList Results;
    App::PropertyPath WorkingDirectory;
    // Attributes are implemented in the FemSolverObjectPython

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderSolver";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

using FemSolverObjectPython = App::FeaturePythonT<FemSolverObject>;

}  // namespace Fem


#endif  // Fem_FemSolverObject_H
