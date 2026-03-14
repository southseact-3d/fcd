#ifndef APP_MERGEDOCUMENTS_H
#define APP_MERGEDOCUMENTS_H

#include <Base/Persistence.h>
#include <fastsignals/signal.h>

namespace zipios
{
class ZipInputStream;
}

namespace App
{
class Document;
class DocumentObject;
class AppExport MergeDocuments: public Base::Persistence
{
public:
    explicit MergeDocuments(App::Document* doc);
    ~MergeDocuments() override;
    bool isVerbose() const
    {
        return verbose;
    }
    void setVerbose(bool on)
    {
        verbose = on;
    }
    unsigned int getMemSize() const override;
    std::vector<App::DocumentObject*> importObjects(std::istream&);
    void importObject(const std::vector<App::DocumentObject*>& o, Base::XMLReader& r);
    void exportObject(const std::vector<App::DocumentObject*>& o, Base::Writer& w);
    void Save(Base::Writer& w) const override;
    void Restore(Base::XMLReader& r) override;
    void SaveDocFile(Base::Writer& w) const override;
    void RestoreDocFile(Base::Reader& r) override;

    const std::map<std::string, std::string>& getNameMap() const
    {
        return nameMap;
    }

private:
    bool guiup {false};
    bool verbose {true};
    zipios::ZipInputStream* stream {nullptr};
    App::Document* appdoc {nullptr};
    std::vector<App::DocumentObject*> objects;
    std::map<std::string, std::string> nameMap;
    using Connection = fastsignals::connection;
    Connection connectExport;
    Connection connectImport;
};

}  // namespace App

#endif  // APP_MERGEDOCUMENTS_H
