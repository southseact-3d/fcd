#ifndef Fem_FemSetGeometryObject_H
#define Fem_FemSetGeometryObject_H

#include "FemSetObject.h"
#include <App/DocumentObject.h>


namespace Fem
{

class FemExport FemSetGeometryObject: public FemSetObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSetGeometryObject);

public:
    /// Constructor
    FemSetGeometryObject();
    ~FemSetGeometryObject() override;

    // returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderSetGeometry";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

}  // namespace Fem


#endif  // Fem_FemSetGeometryObject_H
