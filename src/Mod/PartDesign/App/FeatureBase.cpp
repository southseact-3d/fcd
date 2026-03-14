#include <Standard_Failure.hxx>


#include <App/FeaturePythonPyImp.h>
#include "Body.h"
#include "FeatureBase.h"
#include "FeaturePy.h"

namespace PartDesign
{


PROPERTY_SOURCE(PartDesign::FeatureBase, PartDesign::Feature)

FeatureBase::FeatureBase()
{
    BaseFeature.setScope(App::LinkScope::Global);
    BaseFeature.setStatus(App::Property::Hidden, false);
}

Part::Feature* FeatureBase::getBaseObject(bool) const
{

    return nullptr;
}

short int FeatureBase::mustExecute() const
{

    if (BaseFeature.isTouched()) {
        return 1;
    }

    return PartDesign::Feature::mustExecute();
}


App::DocumentObjectExecReturn* FeatureBase::execute()
{

    if (!BaseFeature.getValue()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "BaseFeature link is not set")
        );
    }

    if (!BaseFeature.getValue()->isDerivedFrom<Part::Feature>()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "BaseFeature must be a Part::Feature")
        );
    }

    auto shape = Part::Feature::getTopoShape(
        BaseFeature.getValue(),
        Part::ShapeOption::ResolveLink | Part::ShapeOption::Transform
    );
    if (shape.isNull()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "BaseFeature has an empty shape")
        );
    }

    Shape.setValue(shape);

    return StdReturn;
}

void FeatureBase::trySetBaseFeatureOfBody()
{
    if (auto body = getFeatureBody()) {
        if (BaseFeature.getValue() && body->BaseFeature.getValue()
            && body->BaseFeature.getValue() != BaseFeature.getValue()) {
            body->BaseFeature.setValue(BaseFeature.getValue());
        }
    }
}

void FeatureBase::onChanged(const App::Property* prop)
{

    // the BaseFeature property should track the Body BaseFeature and vice-versa
    if (prop == &BaseFeature) {
        trySetBaseFeatureOfBody();
    }

    PartDesign::Feature::onChanged(prop);
}

void FeatureBase::onDocumentRestored()
{
    // if the base is not part of a body then show its placement property again
    auto body = getFeatureBody();
    if (!body) {
        Placement.setStatus(App::Property::Hidden, false);
    }
    PartDesign::Feature::onDocumentRestored();
}

}  // namespace PartDesign
