#include "MaterialObject.h"


using namespace App;

PROPERTY_SOURCE(App::MaterialObject, App::DocumentObject)


MaterialObject::MaterialObject()
{
    ADD_PROPERTY_TYPE(Material, (), "Material", Prop_None, "Material key/value map");
}

// Python feature ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(App::MaterialObjectPython, App::MaterialObject)
template<>
const char* App::MaterialObjectPython::getViewProviderName() const
{
    return "Gui::ViewProviderMaterialObjectPython";
}
/// @endcond

// explicit template instantiation
template class AppExport FeaturePythonT<App::MaterialObject>;
}  // namespace App
