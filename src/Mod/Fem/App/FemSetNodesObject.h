#ifndef Fem_FemSetNodesObject_H
#define Fem_FemSetNodesObject_H

#include "FemSetObject.h"
#include <App/DocumentObject.h>
#include <App/PropertyStandard.h>

namespace Fem
{

class FemExport FemSetNodesObject: public FemSetObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSetNodesObject);

public:
    /// Constructor
    FemSetNodesObject();
    ~FemSetNodesObject() override;

    App::PropertyIntegerSet Nodes;

    // returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderSetNodes";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

}  // namespace Fem


#endif  // Fem_FemSetNodesObject_H
