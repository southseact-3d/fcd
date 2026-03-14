#ifndef Fem_FemMeshShapeNetgenObject_H
#define Fem_FemMeshShapeNetgenObject_H

#include "FemMeshShapeObject.h"
#include <App/PropertyStandard.h>

namespace Fem
{

class FemExport FemMeshShapeNetgenObject: public FemMeshShapeBaseObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemMeshShapeNetgenObject);

public:
    /// Constructor
    FemMeshShapeNetgenObject();
    ~FemMeshShapeNetgenObject() override;

    App::PropertyFloat MaxSize;
    App::PropertyFloat MinSize;
    App::PropertyBool SecondOrder;
    App::PropertyEnumeration Fineness;
    App::PropertyFloat GrowthRate;
    App::PropertyInteger NbSegsPerEdge;
    App::PropertyInteger NbSegsPerRadius;
    App::PropertyBool Optimize;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemMeshShapeNetgen";
    }
    App::DocumentObjectExecReturn* execute() override;

    // virtual short mustExecute(void) const;
    // virtual PyObject *getPyObject(void);

    // App::PropertyLink Shape;

protected:
    /// get called by the container when a property has changed
    // virtual void onChanged (const App::Property* prop);
};

}  // namespace Fem


#endif  // Fem_FemMeshShapeNetgenObject_H
