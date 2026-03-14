#include "PreviewExtension.h"

#include <App/DocumentObject.h>

EXTENSION_PROPERTY_SOURCE(Part::PreviewExtension, App::DocumentObjectExtension)
EXTENSION_PROPERTY_SOURCE_TEMPLATE(Part::PreviewExtensionPython, Part::PreviewExtension)

Part::PreviewExtension::PreviewExtension()
{
    initExtensionType(getExtensionClassTypeId());

    EXTENSION_ADD_PROPERTY(PreviewShape, (TopoShape()));

    PreviewShape.setStatus(App::Property::Output, true);
    PreviewShape.setStatus(App::Property::Transient, true);
    PreviewShape.setStatus(App::Property::Hidden, true);
}

void Part::PreviewExtension::updatePreview()
{
    if (_isPreviewFresh) {
        return;
    }

    recomputePreview();

    _isPreviewFresh = true;
}

bool Part::PreviewExtension::mustRecomputePreview() const
{
    return getExtendedObject()->mustRecompute();
}

void Part::PreviewExtension::extensionOnChanged(const App::Property* prop)
{
    DocumentObjectExtension::extensionOnChanged(prop);

    if (mustRecomputePreview()) {
        _isPreviewFresh = false;
    }
}
