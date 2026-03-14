#ifndef GUI_MDIVIEWPYWRAP_H
#define GUI_MDIVIEWPYWRAP_H

#include <memory>
#include <Gui/MDIView.h>
#include <CXX/Objects.hxx>


namespace Gui
{

class MDIViewPyWrapImp;
class GuiExport MDIViewPyWrap: public MDIView
{
    Q_OBJECT

    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /** View constructor
     * Attach the view to the given document. If the document is zero
     * the view will attach to the active document. Be aware, there isn't
     * always an active document.
     */
    explicit MDIViewPyWrap(
        const Py::Object& py,
        Gui::Document* pcDocument,
        QWidget* parent = nullptr,
        Qt::WindowFlags wflags = Qt::WindowFlags()
    );
    /** View destructor
     * Detach the view from the document, if attached.
     */
    ~MDIViewPyWrap() override;

    /// Message handler
    bool onMsg(const char* pMsg, const char** ppReturn) override;
    /// Message handler test
    bool onHasMsg(const char* pMsg) const override;
    /// overwrite when checking on close state
    bool canClose() override;
    PyObject* getPyObject() override;
    /** @name Printing */
    //@{
public Q_SLOTS:
    void print(QPrinter* printer) override;

public:
    /** Print content of view */
    void print() override;
    /** Print to PDF file */
    void printPdf() override;
    /** Show a preview dialog */
    void printPreview() override;
    //@}

    /** @name Undo/Redo actions */
    //@{
    QStringList undoActions() const override;
    QStringList redoActions() const override;
    //@}

private:
    std::unique_ptr<MDIViewPyWrapImp> ptr;
};

}  // namespace Gui

#endif  // GUI_MDIVIEWPYWRAP_H
