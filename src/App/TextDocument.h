#ifndef APP_TEXTDOCUMENT_H
#define APP_TEXTDOCUMENT_H

#include "DocumentObject.h"
#include "PropertyStandard.h"


namespace App
{

class AppExport TextDocument: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::TextDocument);

public:
    using TextSignal = fastsignals::signal<void()>;
    using TextSlot = TextSignal::slot_type;

    PropertyString Text;

    TextDocument();
    ~TextDocument() override = default;

    void onChanged(const Property* prop) override;
    const char* getViewProviderName() const override;

    fastsignals::advanced_connection connectText(const TextSlot& sub);
    fastsignals::connection connectLabel(const TextSlot& sub);

private:
    TextSignal textChanged;
    TextSignal labelChanged;
};

}  // namespace App


#endif
