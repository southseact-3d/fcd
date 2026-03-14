#ifndef GUI_TRANSACTIONOBJECT_H
#define GUI_TRANSACTIONOBJECT_H

#include <App/Transactions.h>

namespace Gui
{
class TransactionViewProvider: public App::TransactionObject
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    TransactionViewProvider();
    ~TransactionViewProvider() override;

    void applyNew(App::Document& Doc, App::TransactionalObject* pcObj) override;
    void applyDel(App::Document& Doc, App::TransactionalObject* pcObj) override;
};

}  // namespace Gui


#endif  // GUI_TRANSACTIONOBJECT_H
