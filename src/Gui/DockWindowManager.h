#ifndef GUI_DOCKWINDOWMANAGER_H
#define GUI_DOCKWINDOWMANAGER_H

#include <QObject>
#include <Base/Bitmask.h>
#include <FCGlobal.h>

class QDockWidget;
class QWidget;

namespace Gui
{

enum class DockWindowOption
{
    // clang-format off
    Hidden = 0,
    Visible = 1,
    HiddenTabbed = 2,
    VisibleTabbed = 3
    // clang-format on
};

using DockWindowOptions = Base::Flags<DockWindowOption>;

struct DockWindowItem
{
    QString name;
    Qt::DockWidgetArea pos;
    bool visibility;
    bool tabbed;
};

class GuiExport DockWindowItems
{
public:
    DockWindowItems();
    ~DockWindowItems();

    void addDockWidget(const char* name, Qt::DockWidgetArea pos, DockWindowOptions option);
    void setDockingArea(const char* name, Qt::DockWidgetArea pos);
    void setVisibility(const char* name, bool v);
    void setVisibility(bool v);
    const QList<DockWindowItem>& dockWidgets() const;

private:
    QList<DockWindowItem> _items;
};

/**
 * Class that manages the widgets inside a QDockWidget.
 * \author Werner Mayer
 */
class GuiExport DockWindowManager: public QObject
{
    Q_OBJECT

public:
    /** Creates the only instance of the DockWindowManager. */
    static DockWindowManager* instance();
    static void destruct();

    bool registerDockWindow(const char* name, QWidget* widget);
    QWidget* unregisterDockWindow(const char* name);
    QWidget* findRegisteredDockWindow(const char* name);
    void setup(DockWindowItems*);

    /// Adds a QDockWidget to the main window and sets \a widget as its widget
    QDockWidget* addDockWindow(
        const char* name,
        QWidget* widget,
        Qt::DockWidgetArea pos = Qt::AllDockWidgetAreas
    );
    /// Removes and destroys the QDockWidget and returns the widget
    /// with name \a name added with @ref addDockWindow.
    QWidget* removeDockWindow(const char* name);
    /// Removes and destroys the QDockWidget that contains \a dock. \a dock
    /// does not get destroyed.
    void removeDockWindow(QWidget* dock);
    /// Returns the widget with name \a name added with @ref addDockWindow.
    /// @note The returned widget is not the QDockWidget instance
    /// returned from @ref addDockWindow. If you want to access the QDockWidget
    /// you get it with parentWidget() of the returned widget.
    QWidget* getDockWindow(const char* name) const;
    /// Returns the QDockWidget container
    QDockWidget* getDockContainer(const char* name) const;
    /// Returns a list of all widgets which set to a QDockWidget.
    QList<QWidget*> getDockWindows() const;
    /// If the corresponding dock widget isn't visible then activate it
    void activate(QWidget* widget);

    void saveState();
    void loadState();
    void retranslate();

    bool isOverlayActivated() const;

private Q_SLOTS:
    /**
     * \internal
     */
    void onDockWidgetDestroyed(QObject*);
    /**
     * \internal
     */
    void onWidgetDestroyed(QObject*);

private:
    QDockWidget* findDockWidget(const QList<QDockWidget*>&, const QString&) const;
    void tabifyDockWidgets(DockWindowItems*);
    void setupOverlayManagement();

    DockWindowManager();
    ~DockWindowManager() override;
    static DockWindowManager* _instance;
    struct DockWindowManagerP* d;
};

}  // namespace Gui

ENABLE_BITMASK_OPERATORS(Gui::DockWindowOption)

#endif  // GUI_DOCKWINDOWMANAGER_H
