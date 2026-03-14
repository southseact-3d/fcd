#ifndef APP_VARSET_H
#define APP_VARSET_H

#include "DocumentObject.h"

namespace App
{

/** A DocumentObject class with the purpose to store variables
 */
class AppExport VarSet: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::VarSet);

public:
    VarSet() = default;
    ~VarSet() override = default;

    const char* getViewProviderName() const override;
};

}  // namespace App
#endif
