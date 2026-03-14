#include <App/DocumentObject.h>

#include "ViewProviderFeature.h"


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderFeature, Gui::ViewProviderDocumentObject)

ViewProviderFeature::ViewProviderFeature()
{
    Base::Color c;
    ADD_PROPERTY(ColourList, (c));
}

ViewProviderFeature::~ViewProviderFeature() = default;

void ViewProviderFeature::attach(App::DocumentObject* pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);
}
