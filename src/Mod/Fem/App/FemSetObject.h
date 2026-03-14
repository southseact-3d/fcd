#ifndef Fem_FemSetObject_H
#define Fem_FemSetObject_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <Mod/Fem/FemGlobal.h>

namespace Fem
{

class FemExport FemSetObject: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemSetObject);

public:
    /// Constructor
    FemSetObject();
    ~FemSetObject() override;

    App::PropertyLink FemMesh;

    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
};

}  // namespace Fem


#endif  // Fem_FemSetObject_H
