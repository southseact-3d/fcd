#include <QKeyEvent>
#include <QKeySequence>


#include "ShortcutListener.h"
#include "ViewProviderSketch.h"


using namespace SketcherGui;

// ******************** ShortcutListener *********************************************//
ShortcutListener::ShortcutListener(ViewProviderSketch* vp)
    : pViewProvider {vp}
{}

ShortcutListener::~ShortcutListener() = default;

bool ShortcutListener::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        QKeyEvent* kevent = static_cast<QKeyEvent*>(event);  // NOLINT
        if (kevent->matches(QKeySequence::Delete)) {
            kevent->accept();
            pViewProvider->deleteSelected();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}
