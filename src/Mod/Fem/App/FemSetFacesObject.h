#ifndef Fem_FemSetFacesObject_H
#define Fem_FemSetFacesObject_H

#include "FemSetObject.h"
#include <App/DocumentObject.h>


namespace Fem
{

class FemExport FemSetFacesObject: public FemSetObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSetFacesObject);

public:
    /// Constructor
    FemSetFacesObject();
    ~FemSetFacesObject() override;

    // returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderSetFaces";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

}  // namespace Fem


#endif  // Fem_FemSetFacesObject_H
