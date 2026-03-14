#ifndef APP_Origin_H
#define APP_Origin_H

#include "Datums.h"

namespace App
{

/** Base class of all geometric document objects.
 */
class AppExport Origin: public App::LocalCoordinateSystem
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::Origin);
    Q_DECLARE_TR_FUNCTIONS(App::Origin)

public:
    /// Constructor
    Origin();
    ~Origin() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderCoordinateSystem";
    }

    bool isOrigin() const override
    {
        return true;
    }
};

}  // namespace App

#endif  // APP_Origin_H
