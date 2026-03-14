#ifndef APP_TRANSACTIONALOBJECT_H
#define APP_TRANSACTIONALOBJECT_H

#include <App/ExtensionContainer.h>

namespace App
{

class Document;
class TransactionObject;

/**
 * @brief Base class of transactional objects.
 *
 * A transactional object provides functionality that is inherited by its
 * children to ensure that these children objects can be targeted by a
 * transaction.
 */
class AppExport TransactionalObject: public App::ExtensionContainer
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::TransactionalObject);

public:
    TransactionalObject();
    ~TransactionalObject() override;

    /// Check if this object is attached to a document.
    virtual bool isAttachedToDocument() const;

    /**
     * @brief Deteach this object from the document.
     *
     * @return the name of the document this object was attached to.
     */
    virtual const char* detachFromDocument();

protected:
    /**
     * @brief Notify the document that a property is about to be changed.
     *
     * This function is called before the property is changed.
     *
     * @param[in,out] doc the document this object is attached to.
     * @param[in] prop the property that is about to be changed.
     */
    void onBeforeChangeProperty(Document* doc, const Property* prop);
};

}  // namespace App


#endif  // APP_TRANSACTIONALOBJECT_H
