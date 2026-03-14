#include "TextDocument.h"
#include "DocumentObject.h"


using namespace App;

PROPERTY_SOURCE(App::TextDocument, App::DocumentObject)

TextDocument::TextDocument()
{
    ADD_PROPERTY_TYPE(Text, (""), 0, App::Prop_Hidden, "Content of the document.");
}

void TextDocument::onChanged(const Property* prop)
{
    if (prop == &Text) {
        textChanged();
    }
    else if (prop == &Label) {
        labelChanged();
    }
    DocumentObject::onChanged(prop);
}

const char* TextDocument::getViewProviderName() const
{
    return "Gui::ViewProviderTextDocument";
}

fastsignals::advanced_connection TextDocument::connectText(const TextSlot& sub)
{
    return textChanged.connect(sub, fastsignals::advanced_tag());
}

fastsignals::connection TextDocument::connectLabel(const TextSlot& sub)
{
    return labelChanged.connect(sub);
}
