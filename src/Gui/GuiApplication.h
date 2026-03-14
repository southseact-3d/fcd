#ifndef GUI_APPLICATION_H
#define GUI_APPLICATION_H

#include "GuiApplicationNativeEventAware.h"
#include <Base/Interpreter.h>  // For Base::SystemExitException
#include <QList>
#include <memory>

class QSessionManager;

namespace Gui
{
/** Override QCoreApplication::notify() to fetch exceptions in Qt widgets
 * properly that are not handled in the event handler or slot.
 */
class GUIApplication: public GUIApplicationNativeEventAware
{
    Q_OBJECT

public:
    explicit GUIApplication(int& argc, char** argv);
    ~GUIApplication() override;

    /**
     * Make forwarding events exception-safe and get more detailed information
     * where an unhandled exception comes from.
     */
    bool notify(QObject* receiver, QEvent* event) override;

    /// Pointer to exceptions caught in Qt event handler
    std::shared_ptr<Base::SystemExitException> caughtException;

public Q_SLOTS:
    void commitData(QSessionManager& manager);

protected:
    bool event(QEvent* event) override;
};

class GUISingleApplication: public GUIApplication
{
    Q_OBJECT

public:
    explicit GUISingleApplication(int& argc, char** argv);
    ~GUISingleApplication() override;

    bool isRunning() const;
    bool sendMessage(const QString& message, int timeout = 5000);

private Q_SLOTS:
    void receiveConnection();
    void processMessages();
    void readFromSocket();

Q_SIGNALS:
    void messageReceived(const QList<QString>&);

private:
    class Private;
    QScopedPointer<Private> d_ptr;
};

class WheelEventFilter: public QObject
{
    Q_OBJECT

public:
    explicit WheelEventFilter(QObject* parent);
    bool eventFilter(QObject* obj, QEvent* ev) override;
};

}  // namespace Gui

#endif  // GUI_APPLICATION_H
