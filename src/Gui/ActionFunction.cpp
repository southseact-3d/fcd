#include <QAction>
#include <QMap>
#include <QPointer>
#include <QTimer>

#include "ActionFunction.h"


using namespace Gui;


namespace Gui
{
class ActionFunctionPrivate
{
public:
    QMap<QAction*, std::function<void()>> triggerMap;
    QMap<QAction*, std::function<void(bool)>> toggleMap;
    QMap<QAction*, std::function<void()>> hoverMap;
};
}  // namespace Gui

ActionFunction::ActionFunction(QObject* parent)
    : QObject(parent)
    , d_ptr(new ActionFunctionPrivate())
{}

ActionFunction::~ActionFunction() = default;

void ActionFunction::trigger(QAction* action, std::function<void()> func)
{
    Q_D(ActionFunction);

    d->triggerMap[action] = func;
    connect(action, &QAction::triggered, this, &ActionFunction::triggered);
}

void ActionFunction::triggered()
{
    Q_D(ActionFunction);

    auto a = qobject_cast<QAction*>(sender());
    QMap<QAction*, std::function<void()>>::iterator it = d->triggerMap.find(a);
    if (it != d->triggerMap.end()) {
        // invoke the class function here
        it.value()();
    }
}

void ActionFunction::toggle(QAction* action, std::function<void(bool)> func)
{
    Q_D(ActionFunction);

    d->toggleMap[action] = func;
    connect(action, &QAction::toggled, this, &ActionFunction::toggled);
}

void ActionFunction::toggled(bool on)
{
    Q_D(ActionFunction);

    auto a = qobject_cast<QAction*>(sender());
    QMap<QAction*, std::function<void(bool)>>::iterator it = d->toggleMap.find(a);
    if (it != d->toggleMap.end()) {
        // invoke the class function here
        it.value()(on);
    }
}

void ActionFunction::hover(QAction* action, std::function<void()> func)
{
    Q_D(ActionFunction);

    d->hoverMap[action] = func;
    connect(action, &QAction::hovered, this, &ActionFunction::hovered);
}

void ActionFunction::hovered()
{
    Q_D(ActionFunction);

    auto a = qobject_cast<QAction*>(sender());
    QMap<QAction*, std::function<void()>>::iterator it = d->hoverMap.find(a);
    if (it != d->hoverMap.end()) {
        // invoke the class function here
        it.value()();
    }
}

// ----------------------------------------------------------------------------

namespace Gui
{
class TimerFunctionPrivate
{
public:
    std::function<void()> timeoutFunc;
    std::function<void(QObject*)> timeoutFuncQObject;
    std::function<void(QVariant)> timeoutFuncQVariant;
    bool autoDelete;
    QPointer<QObject> argQObject;
    QVariant argQVariant;
};
}  // namespace Gui

TimerFunction::TimerFunction(QObject* parent)
    : QObject(parent)
    , d_ptr(new TimerFunctionPrivate())
{
    d_ptr->autoDelete = false;
}

TimerFunction::~TimerFunction() = default;

void TimerFunction::setFunction(std::function<void()> func)
{
    Q_D(TimerFunction);
    d->timeoutFunc = func;
}

void TimerFunction::setFunction(std::function<void(QObject*)> func, QObject* args)
{
    Q_D(TimerFunction);
    d->timeoutFuncQObject = func;
    d->argQObject = args;
}

void TimerFunction::setFunction(std::function<void(QVariant)> func, QVariant args)
{
    Q_D(TimerFunction);
    d->timeoutFuncQVariant = func;
    d->argQVariant = args;
}

void TimerFunction::setAutoDelete(bool on)
{
    Q_D(TimerFunction);
    d->autoDelete = on;
}

void TimerFunction::timeout()
{
    Q_D(TimerFunction);
    if (d->timeoutFunc) {
        d->timeoutFunc();
    }
    else if (d->timeoutFuncQObject) {
        d->timeoutFuncQObject(d->argQObject);
    }
    else if (d->timeoutFuncQVariant) {
        d->timeoutFuncQVariant(d->argQVariant);
    }
    if (d->autoDelete) {
        deleteLater();
    }
}

void TimerFunction::singleShot(int ms)
{
    QTimer::singleShot(ms, this, &Gui::TimerFunction::timeout);
}

#include "moc_ActionFunction.cpp"
