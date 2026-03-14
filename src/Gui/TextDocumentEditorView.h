#ifndef GUI_TEXTDOCUMENTEDITORVIEW_H
#define GUI_TEXTDOCUMENTEDITORVIEW_H

#include <QPlainTextEdit>

#include <App/TextDocument.h>
#include <Gui/MDIView.h>


namespace Gui
{

class GuiExport TextDocumentEditorView: public MDIView
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TextDocumentEditorView)
    TYPESYSTEM_HEADER_WITH_OVERRIDE();  // NOLINT

public:
    TextDocumentEditorView(App::TextDocument* textDocument, QPlainTextEdit* editor, QWidget* parent);
    ~TextDocumentEditorView() override;
    const char* getName() const override
    {
        return "TextDocumentEditorView";
    }
    bool onMsg(const char* msg, const char** output) override;
    bool onHasMsg(const char* msg) const override;

    QPlainTextEdit* getEditor() const
    {
        return editor;
    }
    App::TextDocument* getTextObject() const
    {
        return textDocument;
    }
    QStringList undoActions() const override;
    QStringList redoActions() const override;

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void setupEditor();
    void setupConnection();
    void saveToObject();
    void sourceChanged();
    void textChanged();
    void labelChanged();
    void refresh();

private:
    QPlainTextEdit* const editor;
    App::TextDocument* const textDocument;
    fastsignals::advanced_connection textConnection;
    fastsignals::connection labelConnection;
    bool aboutToClose = false;
};

}  // namespace Gui

#endif
