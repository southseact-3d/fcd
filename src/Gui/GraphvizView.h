#ifndef GUI_GRAPHVIZVIEW_H
#define GUI_GRAPHVIZVIEW_H

#include <fastsignals/signal.h>

#include "MDIView.h"

class QGraphicsScene;
class QGraphicsView;
class QSvgRenderer;
class QGraphicsSvgItem;
class GraphicsViewZoom;

namespace Gui
{

class GraphvizWorker;

class GuiExport GraphvizView: public MDIView
{
    Q_OBJECT

public:
    explicit GraphvizView(App::Document& _doc, QWidget* parent = nullptr);
    ~GraphvizView() override;

    QByteArray exportGraph(const QString& filter);

    /// Message handler
    bool onMsg(const char* pMsg, const char** ppReturn) override;
    /// Message handler test
    bool onHasMsg(const char* pMsg) const override;
    /** @name Printing */
    //@{
    void print(QPrinter* printer) override;
    /** Print content of view */
    void print() override;
    /** Print to PDF file */
    void printPdf() override;
    /** Show a preview dialog */
    void printPreview() override;
    //@}

private Q_SLOTS:
    void svgFileRead(const QByteArray& data);
    void error();
    void done();

private:
    void updateSvgItem(const App::Document& doc);
    void disconnectSignals();

    const App::Document& doc;
    std::string graphCode;
    QGraphicsScene* scene;
    QGraphicsView* view;
    GraphicsViewZoom* zoomer;
    QGraphicsSvgItem* svgItem;
    QSvgRenderer* renderer;
    GraphvizWorker* thread;
    int nPending;

    using Connection = fastsignals::scoped_connection;
    Connection recomputeConnection;
    Connection undoConnection;
    Connection redoConnection;
};

}  // namespace Gui

#endif  // GUI_GRAPHVIZVIEW_H
