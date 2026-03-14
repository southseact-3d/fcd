#include "TransactionalObject.h"
#include "Document.h"


using namespace App;


PROPERTY_SOURCE_ABSTRACT(App::TransactionalObject, App::ExtensionContainer)

TransactionalObject::TransactionalObject() = default;

TransactionalObject::~TransactionalObject() = default;

bool TransactionalObject::isAttachedToDocument() const
{
    return false;
}

const char* TransactionalObject::detachFromDocument()
{
    return "";
}

void TransactionalObject::onBeforeChangeProperty(Document* doc, const Property* prop)
{
    doc->onBeforeChangeProperty(this, prop);
}
