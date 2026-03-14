#include <iostream>

#include "VarSet.h"
#include "DocumentObject.h"

using namespace App;

PROPERTY_SOURCE(App::VarSet, App::DocumentObject)

const char* VarSet::getViewProviderName() const
{
    return "Gui::ViewProviderVarSet";
}
