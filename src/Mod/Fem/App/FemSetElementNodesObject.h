#ifndef Fem_FemSetElementNodesObject_H
#define Fem_FemSetElementNodesObject_H

#include "FemSetObject.h"
#include <App/DocumentObject.h>
#include <App/PropertyStandard.h>

namespace Fem
{

class FemExport FemSetElementNodesObject: public FemSetObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSetElementNodesObject);

public:
    /// Constructor
    FemSetElementNodesObject();
    ~FemSetElementNodesObject() override;

    App::PropertyIntegerSet Elements;

    // returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderSetElementNodes";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
    static std::string getElementName()
    {
        return "ElementsSet";
    }
};

}  // namespace Fem


#endif  // Fem_FemSetElementNodesObject_H
