#ifndef APP_DOCUMENTOBJECTFILEINCLUDED_H
#define APP_DOCUMENTOBJECTFILEINCLUDED_H

#include "DocumentObject.h"
#include "PropertyFile.h"


namespace App
{

class AppExport DocumentObjectFileIncluded: public DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::DocumentObjectFileIncluded);

public:
    /// Constructor
    DocumentObjectFileIncluded();
    ~DocumentObjectFileIncluded() override;


    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderDocumentObject";
    }

    /// Properties
    PropertyFileIncluded File;
};

}  // namespace App


#endif  // APP_DOCUMENTOBJECTFILEINCLUDED_H
