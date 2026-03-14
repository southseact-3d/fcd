#ifndef APP_MaterialObject_H
#define APP_MaterialObject_H

#include "DocumentObject.h"
#include "FeaturePython.h"


namespace App
{

class AppExport MaterialObject: public DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::MaterialObject);

public:
    /// Constructor
    MaterialObject();

    App::PropertyMap Material;


    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderMaterialObject";
    }
};

using MaterialObjectPython = App::FeaturePythonT<MaterialObject>;


}  // namespace App


#endif  // APP_MaterialObject_H
