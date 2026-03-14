#ifndef FEM_CONSTRAINTTransform_H
#define FEM_CONSTRAINTTransform_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintTransform: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintTransform);

public:
    /// Constructor
    ConstraintTransform();

    // Read-only (calculated values). These trigger changes in the ViewProvider
    App::PropertyLinkSubList RefDispl;
    App::PropertyLinkList NameDispl;
    App::PropertyVector BasePoint;
    App::PropertyVector Axis;

    App::PropertyRotation Rotation;
    App::PropertyEnumeration TransformType;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override;

protected:
    void handleChangedPropertyName(
        Base::XMLReader& reader,
        const char* typeName,
        const char* propName
    ) override;
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTTransform_H
