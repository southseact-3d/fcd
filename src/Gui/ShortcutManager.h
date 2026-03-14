#ifndef GUI_SHORTCUT_MANAGER_H
#define GUI_SHORTCUT_MANAGER_H

#include <unordered_map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <QAction>
#include <QKeySequence>
#include <QPointer>
#include <QTimer>

#include <Base/Parameter.h>

namespace Gui
{

class Command;
namespace bmi = boost::multi_index;

class GuiExport ShortcutManager: public QObject, public ParameterGrp::ObserverType
{
    Q_OBJECT

public:
    ShortcutManager();
    ~ShortcutManager() override;

    static ShortcutManager* instance();
    static void destroy();

    void OnChange(Base::Subject<const char*>&, const char* reason) override;

    /// Clear all user defined shortcut
    void resetAll();
    /// Clear the user defined shortcut of a given command
    void reset(const char* cmd);
    /// Set shortcut of a given command
    void setShortcut(const char* cmd, const char* accel);
    /** Get shortcut of a given command
     * @param cmd: command name
     * @param defaultAccel: default shortcut
     */
    QString getShortcut(const char* cmd, const char* defaultAccel = nullptr);

    /// Return actions having a given shortcut in order of decreasing priority
    std::vector<std::pair<QByteArray, QAction*>> getActionsByShortcut(const QKeySequence& shortcut);

    /// Set properties for a given list of actions in order of decreasing priority
    void setPriorities(const std::vector<QByteArray>& actions);

    /** Set the priority of a given command
     * @param cmd: command name
     * @param priority: priority of the command, bigger value means higher priority
     */
    void setPriority(const char* cmd, int priority);

    /// Get the priority of a given command
    int getPriority(const char* cmd);

    /** Set the top priority of a given command
     * Make the given command the top priority of all commands.
     *
     * @param cmd: command name
     */
    void setTopPriority(const char* cmd);

Q_SIGNALS:
    void shortcutChanged(const char* name, const QKeySequence& oldShortcut);
    void actionShortcutChanged(QAction*, const QKeySequence& oldShortcut);
    void priorityChanged(const char* name, int priority);

protected:
    bool eventFilter(QObject*, QEvent* ev) override;
    bool checkShortcut(QObject* o, const QKeySequence& key);
    void onTimer();

private:
    ParameterGrp::handle hShortcuts;
    ParameterGrp::handle hPriorities;
    ParameterGrp::handle hSetting;
    bool busy = false;

    struct ActionKey
    {
        QKeySequence shortcut;
        QByteArray name;
        explicit ActionKey(const QKeySequence& shortcut, const char* name = "")
            : shortcut(shortcut)
            , name(name)
        {}
        bool operator<(const ActionKey& other) const
        {
            if (shortcut > other.shortcut) {
                return false;
            }
            if (shortcut < other.shortcut) {
                return true;
            }
            return name < other.name;
        }
    };
    struct ActionData
    {
        ActionKey key;
        intptr_t pointer;
        QPointer<QAction> action;

        explicit ActionData(QAction* action, const char* name = "")
            : key(action->shortcut(), name)
            , pointer(reinterpret_cast<intptr_t>(action))
            , action(action)
        {}
    };
    bmi::multi_index_container<
        ActionData,
        bmi::indexed_by<
            // hashed index on ActionData::Action pointer
            bmi::hashed_unique<bmi::member<ActionData, intptr_t, &ActionData::pointer>>,
            // ordered index on shortcut + name
            bmi::ordered_non_unique<bmi::member<ActionData, ActionKey, &ActionData::key>>>>
        actionMap;

    std::unordered_map<std::string, int> priorities;
    int topPriority;

    struct ActionInfo
    {
        QPointer<QAction> action;
        int seq_length;
        int priority;

        ActionInfo(QAction* action, int l, int p)
            : action(action)
            , seq_length(l)
            , priority(p)
        {}
    };
    std::vector<ActionInfo> pendingActions;

    QKeySequence pendingSequence;

    QPointer<QWidget> lastFocus;

    QTimer timer;
    int timeout;
};

}  // namespace Gui

#endif  // GUI_SHORTCUT_MANAGER_H
