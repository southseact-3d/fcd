#ifndef GUI_MERGEDOCUMENTS_H
#define GUI_MERGEDOCUMENTS_H

#include <Base/Persistence.h>
#include <map>
#include <vector>
#include <fastsignals/signal.h>

namespace zipios
{
class ZipInputStream;
}
namespace App
{
class Document;
class DocumentObject;
}  // namespace App

namespace Gui
{
class Document;
class GuiExport MergeDocuments: public Base::Persistence
{
public:
    explicit MergeDocuments(App::Document* doc);
    ~MergeDocuments() override;
    unsigned int getMemSize() const override;
    std::vector<App::DocumentObject*> importObjects(std::istream&);
    void importObject(const std::vector<App::DocumentObject*>& o, Base::XMLReader& r);
    void exportObject(const std::vector<App::DocumentObject*>& o, Base::Writer& w);
    void Save(Base::Writer& w) const override;
    void Restore(Base::XMLReader& r) override;
    void SaveDocFile(Base::Writer& w) const override;
    void RestoreDocFile(Base::Reader& r) override;

private:
    zipios::ZipInputStream* stream {nullptr};
    App::Document* appdoc;
    Gui::Document* document;
    std::vector<App::DocumentObject*> objects;
    std::map<std::string, std::string> nameMap;
    using Connection = fastsignals::connection;
    Connection connectExport;
    Connection connectImport;
};

}  // namespace Gui

#endif  // GUI_MERGEDOCUMENTS_H
