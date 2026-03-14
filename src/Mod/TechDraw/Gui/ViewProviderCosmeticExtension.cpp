# include <cmath>

#include <Mod/TechDraw/App/CosmeticExtension.h>
#include "ViewProviderCosmeticExtension.h"

using namespace TechDrawGui;

EXTENSION_PROPERTY_SOURCE(TechDrawGui::ViewProviderCosmeticExtension, Gui::ViewProviderExtension)


ViewProviderCosmeticExtension::ViewProviderCosmeticExtension()
{
    initExtensionType(ViewProviderCosmeticExtension::getExtensionClassTypeId());
}

QIcon ViewProviderCosmeticExtension::extensionMergeGreyableOverlayIcons(const QIcon & orig) const
{
    QIcon mergedicon = orig;

    return mergedicon;
}

namespace Gui {
    EXTENSION_PROPERTY_SOURCE_TEMPLATE(TechDrawGui::ViewProviderCosmeticExtensionPython, TechDrawGui::ViewProviderCosmeticExtension)

// explicit template instantiation
    template class TechDrawGuiExport ViewProviderExtensionPythonT<TechDrawGui::ViewProviderCosmeticExtension>;
}
