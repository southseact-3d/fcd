#ifndef GUI_MENUMANAGER_H
#define GUI_MENUMANAGER_H

#include <string>
#include <QList>
#include <QString>
#include <FCGlobal.h>


class QAction;
class QMenu;

namespace Gui
{

class MainWindow;

class GuiExport MenuItem
{
public:
    MenuItem();
    explicit MenuItem(MenuItem*);
    ~MenuItem();

    void setCommand(const std::string&);
    std::string command() const;

    bool hasItems() const;
    MenuItem* findItem(const std::string&);
    MenuItem* findParentOf(const std::string&);
    MenuItem* copy() const;
    uint count() const;

    void appendItem(MenuItem*);
    bool insertItem(MenuItem*, MenuItem*);
    MenuItem* afterItem(MenuItem*) const;
    void removeItem(MenuItem*);
    void clear();

    MenuItem& operator<<(MenuItem* item);
    MenuItem& operator<<(const std::string& command);
    QList<MenuItem*> getItems() const;

private:
    std::string _name;
    QList<MenuItem*> _items;
};

/**
 * The MenuManager class is responsible for the creation of menus and context menus and appending
 * them to the main window.
 * @see ToolBoxManager
 * @see ToolBarManager
 * @author Werner Mayer
 */
class GuiExport MenuManager
{
public:
    /** Sets up the menus of a given workbench. */
    void setup(MenuItem*) const;
    /// sets up a context menu out of item
    void setupContextMenu(MenuItem* item, QMenu& menu) const;
    void retranslate() const;

    /// The one and only instance.
    static MenuManager* getInstance();
    static void destruct();

protected:
    MenuManager();
    ~MenuManager();

private:
    void setup(MenuItem*, QMenu*) const;
    void retranslate(QMenu*) const;
    QAction* findAction(const QList<QAction*>&, const QString&) const;
    QList<QAction*> findActions(const QList<QAction*>&, const QString&) const;

private:
    static MenuManager* _instance;

    friend class MainWindow;
};

}  // namespace Gui


#endif  // GUI_MENUMANAGER_H
