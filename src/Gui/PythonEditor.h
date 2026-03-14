#ifndef GUI_PYTHONEDITOR_H
#define GUI_PYTHONEDITOR_H

#include "SyntaxHighlighter.h"
#include "TextEdit.h"


namespace Gui
{

class PythonSyntaxHighlighter;
class PythonSyntaxHighlighterP;

/**
 * Python text editor with syntax highlighting.
 * \author Werner Mayer
 */
class GuiExport PythonEditor: public PythonTextEditor
{
    Q_OBJECT

public:
    explicit PythonEditor(QWidget* parent = nullptr);
    ~PythonEditor() override;

    void OnChange(Base::Subject<const char*>& rCaller, const char* rcReason) override;
    void toggleBreakpoint();
    void showDebugMarker(int line);
    void hideDebugMarker();

public Q_SLOTS:
    /** Inserts a '#' at the beginning of each selected line or the current line if
     * nothing is selected
     */
    void onComment();
    /**
     * Removes the leading '#' from each selected line or the current line if
     * nothing is selected. In case a line hasn't a leading '#' then
     * this line is skipped.
     */
    void onUncomment();
    void onExecuteInConsole();
    void setFileName(const QString&);
    void startDebug();

protected:
    /** Pops up the context menu with some extensions */
    void contextMenuEvent(QContextMenuEvent* e) override;
    void drawMarker(int line, int x, int y, QPainter*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    // PythonSyntaxHighlighter* pythonSyntax;
    struct PythonEditorP* d;
};

/**
 * Syntax highlighter for Python.
 * \author Werner Mayer
 */
class GuiExport PythonSyntaxHighlighter: public SyntaxHighlighter
{
public:
    explicit PythonSyntaxHighlighter(QObject* parent);
    ~PythonSyntaxHighlighter() override;

    void highlightBlock(const QString& text) override;

private:
    PythonSyntaxHighlighterP* d;
};

}  // namespace Gui

#endif  // GUI_PYTHONEDITOR_H
