#ifndef SHEETOBSERVER_H
#define SHEETOBSERVER_H

#include <App/DocumentObserver.h>

namespace Spreadsheet
{

class PropertySheet;

// SheetObserver is obsolete as PropertySheet is now derived from PropertyLinkBase
class SheetObserver: public App::DocumentObserver
{
public:
    SheetObserver(App::Document* document, PropertySheet* _sheet);
    ~SheetObserver() override = default;
    void slotCreatedObject(const App::DocumentObject& Obj) override;
    void slotDeletedObject(const App::DocumentObject& Obj) override;
    void slotChangedObject(const App::DocumentObject& Obj, const App::Property& Prop) override;
    void ref();
    bool unref();
    App::Document* getDocument() const
    {
        return App::DocumentObserver::getDocument();
    }

private:
    std::set<std::string> isUpdating;
    unsigned int refCount {1};
    PropertySheet* sheet;
};

}  // namespace Spreadsheet

#endif  // SHEETOBSERVER_H
