#include <Inventor/events/SoKeyboardEvent.h>
#include <QApplication>
#include <QEvent>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


#include "ViewProviderSketch.h"

#include "DrawSketchKeyboardManager.h"

using namespace SketcherGui;


DrawSketchKeyboardManager::DrawSketchKeyboardManager()
    : QObject(nullptr)
    , keyMode(KeyboardEventHandlingMode::DSHControl)
{
    // get the active viewer, so that we can send it key events
    auto doc = Gui::Application::Instance->activeDocument();

    if (doc) {
        auto temp = dynamic_cast<Gui::View3DInventor*>(doc->getActiveView());
        if (temp) {
            vpViewer = temp->getViewer();
            keyMode = KeyboardEventHandlingMode::ViewProvider;
        }
    }

    timer.setSingleShot(true);

    QObject::connect(&timer, &QTimer::timeout, [this]() { onTimeOut(); });
}

bool DrawSketchKeyboardManager::isMode(KeyboardEventHandlingMode mode)
{
    return mode == keyMode;
}

DrawSketchKeyboardManager::KeyboardEventHandlingMode DrawSketchKeyboardManager::getMode()
{
    return keyMode;
}

bool DrawSketchKeyboardManager::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);

    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        /*If a key shortcut is required to work on sketcher when a tool using ui controls is being
         * used, then you have to add this key to the below section such that the spinbox doesn't
         * keep the keypress event for itself. Note if you want the event to be handled by the
         * spinbox too, you can return false.*/

        auto keyEvent = static_cast<QKeyEvent*>(event);

        detectKeyboardEventHandlingMode(keyEvent);  // determine the handler

        if (vpViewer && isMode(KeyboardEventHandlingMode::ViewProvider)) {
            return QApplication::sendEvent(vpViewer, keyEvent);
        }

        return false;  // do not intercept the event and feed it to the widget
    }

    return false;
}

void DrawSketchKeyboardManager::detectKeyboardEventHandlingMode(QKeyEvent* keyEvent)
{
    QRegularExpression rx(QStringLiteral("^[0-9]$"));
    auto match = rx.match(keyEvent->text());
    if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return
        || keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab
        || keyEvent->key() == Qt::Key_Minus || keyEvent->key() == Qt::Key_Period
        || keyEvent->key() == Qt::Key_Comma
        || match.hasMatch()
        // double check for backspace as there may be windows/unix inconsistencies
        || keyEvent->key() == Qt::Key_Backspace || keyEvent->matches(QKeySequence::Backspace)
        || keyEvent->matches(QKeySequence::Delete)) {
        keyMode = KeyboardEventHandlingMode::DSHControl;
        timer.start(timeOutValue);
    }
}

void DrawSketchKeyboardManager::onTimeOut()
{
    keyMode = KeyboardEventHandlingMode::ViewProvider;
}

/// sets the timeout to the amount of milliseconds.
void DrawSketchKeyboardManager::setTimeOut(int milliseconds)
{
    timeOutValue = milliseconds;
}

// returns the current timeout amount
int DrawSketchKeyboardManager::timeOut()
{
    return timeOutValue;
}


#include "moc_DrawSketchKeyboardManager.cpp"
