#ifndef Fem_FemMeshShapeObject_H
#define Fem_FemMeshShapeObject_H

#include "FemMeshObject.h"


namespace Fem
{

class FemExport FemMeshShapeBaseObject: public FemMeshObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemMeshShapeBaseObject);

public:
    /// Constructor
    FemMeshShapeBaseObject();
    ~FemMeshShapeBaseObject() override;

    App::PropertyLink Shape;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemMeshShapeBase";
    }
};


class FemExport FemMeshShapeObject: public FemMeshShapeBaseObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemMeshShapeObject);

public:
    /// Constructor
    FemMeshShapeObject();
    ~FemMeshShapeObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemMeshShape";
    }
    App::DocumentObjectExecReturn* execute() override;

    // virtual short mustExecute(void) const;
    // virtual PyObject *getPyObject(void);

protected:
    /// get called by the container when a property has changed
    // virtual void onChanged (const App::Property* prop);
};

using FemMeshShapeBaseObjectPython = App::FeaturePythonT<FemMeshShapeBaseObject>;

}  // namespace Fem


#endif  // Fem_FemMeshShapeObject_H
