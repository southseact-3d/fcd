#ifndef DAGVIEW_H
#define DAGVIEW_H

#include <memory>
#include <QGraphicsView>
#include <fastsignals/signal.h>

#include <Gui/DockWindow.h>
#include <Gui/Document.h>
#include <Gui/Selection/Selection.h>

#include "DAGModel.h"


namespace Gui
{
namespace DAG
{
//! @brief view for DAG viewer
class View: public QGraphicsView, public SelectionObserver
{
    Q_OBJECT
public:
    explicit View(QWidget* parentIn = nullptr);
    ~View() override;

public Q_SLOTS:
    void awakeSlot();  //!< hooked up to event dispatcher for update when idle.

private:
    void onSelectionChanged(const SelectionChanges& msg) override;

    void slotActiveDocument(const Gui::Document& documentIn);
    void slotDeleteDocument(const Gui::Document& documentIn);

    using ModelMap = std::map<const Gui::Document*, std::shared_ptr<Model>>;
    ModelMap modelMap;
    fastsignals::scoped_connection conActive;
    fastsignals::scoped_connection conDelete;
};

//! @brief dock window for DAG viewer
class DockWindow: public Gui::DockWindow
{
    Q_OBJECT
public:
    explicit DockWindow(Gui::Document* gDocumentIn = nullptr, QWidget* parent = nullptr);
    ~DockWindow() override = default;

private:
    View* dagView;
};
}  // namespace DAG
}  // namespace Gui

#endif  // DAGVIEW_H
