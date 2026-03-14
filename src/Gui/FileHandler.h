#ifndef GUI_FILE_HANDLER_H
#define GUI_FILE_HANDLER_H

#include <QString>
#include <string>

namespace App
{
class Document;
}

namespace Gui
{

class FileHandler
{
public:
    explicit FileHandler(const QString& filename);
    bool openFile();
    bool importFile(const std::string& document);
    QString extension() const;

private:
    bool activateEditor();
    App::Document* createDocumentIfNeeded();
    App::Document* getOrCreateDocument();
    App::Document* getOrCreateDocument(const std::string& document);
    bool openInternal();
    void openInternal(const char* type, const char* prop);
    void openInventor();
    void openVRML();
    void openImage();
    void openPython();

private:
    QString filename;
    std::string docname;
};

}  // namespace Gui

#endif  // GUI_FILE_HANDLER_H
