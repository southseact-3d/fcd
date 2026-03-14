#include "Annotation.h"

using namespace App;

PROPERTY_SOURCE(App::Annotation, App::DocumentObject)


Annotation::Annotation()
{
    ADD_PROPERTY(LabelText, (""));
    ADD_PROPERTY(Position, (Base::Vector3d()));
}

Annotation::~Annotation() = default;

// --------------------------------------------------

PROPERTY_SOURCE(App::AnnotationLabel, App::DocumentObject)


AnnotationLabel::AnnotationLabel()
{
    ADD_PROPERTY_TYPE(LabelText, (""), "Label", Prop_Output, "Text label of the annotation");
    ADD_PROPERTY_TYPE(BasePosition, (Base::Vector3d()), "Label", Prop_Output, "Base position");
    ADD_PROPERTY_TYPE(TextPosition, (Base::Vector3d()), "Label", Prop_Output, "Text position");
}

AnnotationLabel::~AnnotationLabel() = default;
