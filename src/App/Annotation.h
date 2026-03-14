#ifndef APP_ANNOTATION_H
#define APP_ANNOTATION_H

#include "DocumentObject.h"
#include "PropertyGeo.h"
#include "PropertyStandard.h"


namespace App
{

class AppExport Annotation: public DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::Annotation);

public:
    /// Constructor
    Annotation();
    ~Annotation() override;

    App::PropertyStringList LabelText;
    App::PropertyVector Position;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderAnnotation";
    }
};

class AppExport AnnotationLabel: public DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::AnnotationLabel);

public:
    /// Constructor
    AnnotationLabel();
    ~AnnotationLabel() override;

    App::PropertyStringList LabelText;
    App::PropertyVector BasePosition;
    App::PropertyVector TextPosition;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderAnnotationLabel";
    }
};

}  // namespace App


#endif  // APP_ANNOTATION_H
