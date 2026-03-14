#include "PropertySheet.h"
#include "SheetObserver.h"


using namespace Spreadsheet;
using namespace App;

/**
 * The SheetObserver constructor.
 *
 * @param document The Document we are observing
 * @param _sheet   The sheet owning this observer.
 *
 */

SheetObserver::SheetObserver(App::Document* document, PropertySheet* _sheet)
    : DocumentObserver(document)
    , sheet(_sheet)
{}

/**
 * Invalidate cells that depend on this document object.
 *
 */

void SheetObserver::slotCreatedObject(const DocumentObject& Obj)
{
    sheet->invalidateDependants(&Obj);
}

/**
 * Invalidate cells that depend on this document object.
 *
 */

void SheetObserver::slotDeletedObject(const DocumentObject& Obj)
{
    sheet->invalidateDependants(&Obj);
    sheet->deletedDocumentObject(&Obj);
}

/**
 * Invoke the sheets recomputeDependants when a change to a Property occurs.
 *
 */

void SheetObserver::slotChangedObject(const DocumentObject& Obj, const Property& Prop)
{
    if (&Prop == &Obj.Label) {
    }
    else {
        const char* name = Obj.getPropertyName(&Prop);

        if (!name) {
            return;
        }

        if (isUpdating.find(name) != isUpdating.end()) {
            return;
        }

        isUpdating.insert(name);
        sheet->recomputeDependants(&Obj, Prop.getName());
        isUpdating.erase(name);
    }
}

/**
 * Increase reference count.
 *
 */

void SheetObserver::ref()
{
    refCount++;
}

/**
 * Decrease reference count.
 *
 */

bool SheetObserver::unref()
{
    refCount--;
    return refCount;
}
