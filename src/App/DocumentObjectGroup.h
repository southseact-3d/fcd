#ifndef APP_DOCUMENTOBJECTGROUP_H
#define APP_DOCUMENTOBJECTGROUP_H

#include "DocumentObject.h"
#include "FeaturePython.h"
#include "GroupExtension.h"


namespace App
{

class AppExport DocumentObjectGroup: public DocumentObject, public GroupExtension
{

    PROPERTY_HEADER_WITH_EXTENSIONS(App::DocumentObjectGroup);

public:
    /// Constructor
    DocumentObjectGroup();
    ~DocumentObjectGroup() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderDocumentObjectGroup";
    }

    PyObject* getPyObject() override;
};

using DocumentObjectGroupPython = App::FeaturePythonT<DocumentObjectGroup>;


}  // namespace App


#endif  // APP_DOCUMENTOBJECTGROUP_H
