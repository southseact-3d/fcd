#ifndef APP_INVENTOROBJECT_H
#define APP_INVENTOROBJECT_H

#include "GeoFeature.h"
#include "PropertyStandard.h"


namespace App
{

class AppExport InventorObject: public GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::InventorObject);

public:
    /// Constructor
    InventorObject();
    ~InventorObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderInventorObject";
    }
    DocumentObjectExecReturn* execute() override
    {
        return DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;

    PropertyString Buffer;
    PropertyString FileName;
};

}  // namespace App


#endif  // APP_INVENTOROBJECT_H
