#ifndef Fem_FemSetElementsObjec_H
#define Fem_FemSetElementsObjec_H

#include "FemSetObject.h"
#include <App/DocumentObject.h>


namespace Fem
{

class FemExport FemSetElementsObject: public FemSetObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSetElementsObject);

public:
    /// Constructor
    FemSetElementsObject();
    ~FemSetElementsObject() override;

    // returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderSetElements";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

}  // namespace Fem


#endif  // Fem_FemSetElementsObjec_H
