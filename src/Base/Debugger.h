#ifndef BASE_DEBUGGER_H
#define BASE_DEBUGGER_H

#include <QEventLoop>
#include <QObject>
#ifndef FC_GLOBAL_H
# include <FCGlobal.h>
#endif

namespace Base
{
/**
  This is a utility class to break the application at a point to inspect e.g. the result of
  an algorithm.
  You usually use it like this
  \code
    ...
    Base::Debugger dbg;
    dbg.attach();
    dbg.exec();
    ...
  \endcode
  Or you can connect it with a button and let the user click it in order to continue.
  \code
    QPushButton* btn = new QPushButton();
    btn->setText("Continue");
    btn->show();
    Base::Debugger dbg;
    connect(btn, &QPushButton::clicked, &dbg, &Debugger::quit);
    dbg.exec();
  \endcode
 \author Werner Mayer
 */
class BaseExport Debugger: public QObject
{
    Q_OBJECT

public:
    explicit Debugger(QObject* parent = nullptr);
    ~Debugger() override;

    Debugger(const Debugger&) = delete;
    Debugger(Debugger&&) = delete;
    Debugger& operator=(const Debugger&) = delete;
    Debugger& operator=(Debugger&&) = delete;

    void attach();
    void detach();
    bool eventFilter(QObject* obj, QEvent* event) override;
    int exec();

public Q_SLOTS:
    void quit();

private:
    bool isAttached {false};
    QEventLoop loop;
};

}  // namespace Base

#endif  // BASE_DEBUGGER_H
