#include "Application.h"
#include "Document.h"
#include "TransactionObject.h"
#include "ViewProviderDocumentObject.h"


using namespace Gui;


TYPESYSTEM_SOURCE_ABSTRACT(Gui::TransactionViewProvider, App::TransactionObject)

TransactionViewProvider::TransactionViewProvider() = default;

TransactionViewProvider::~TransactionViewProvider() = default;

void TransactionViewProvider::applyNew(App::Document& Doc, App::TransactionalObject* pcObj)
{
    if (status == New) {
        Gui::Document* doc = Application::Instance->getDocument(&Doc);
        if (doc) {
            doc->addViewProvider(static_cast<ViewProviderDocumentObject*>(pcObj));
        }
    }
}

void TransactionViewProvider::applyDel(App::Document& Doc, App::TransactionalObject* pcObj)
{
    // nothing to do here
    Q_UNUSED(Doc);
    Q_UNUSED(pcObj);
}
