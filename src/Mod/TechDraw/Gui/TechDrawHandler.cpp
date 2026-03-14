#include <cmath>

#include <QGuiApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

#include <Inventor/events/SoKeyboardEvent.h>

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Application.h>
#include <Gui/CommandT.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "MDIViewPage.h"
#include "QGVPage.h"
#include "TechDrawHandler.h"


using namespace TechDrawGui;

/**************************** TechDrawHandler *******************************************/

TechDrawHandler::TechDrawHandler() : Gui::ToolHandler(), viewPage(nullptr)
{}

TechDrawHandler::~TechDrawHandler()
{}

void TechDrawHandler::activate(QGVPage* vp)
{
    auto* mdi = qobject_cast<MDIViewPage*>(Gui::getMainWindow()->activeWindow());
    if (!mdi) {
        return;
    }
    mdi->enableContextualMenu(false);

    viewPage = vp;

    if (!Gui::ToolHandler::activate()) {
        viewPage->deactivateHandler();
    }
}

void TechDrawHandler::deactivate()
{
    Gui::ToolHandler::deactivate();

    // The context menu event of MDIViewPage comes after the tool is deactivated.
    // So to prevent the menu from appearing when the tool is cleared by right mouse click
    // we set a small timer.
    QTimer::singleShot(100, []() { // 100 milliseconds delay
        auto* mdi = qobject_cast<MDIViewPage*>(Gui::getMainWindow()->activeWindow());
        if (!mdi) {
            return;
        }
        mdi->enableContextualMenu(true);
    });
}

void TechDrawHandler::keyReleaseEvent(QKeyEvent* event)
{
    // the default behaviour is to quit - specific handler categories may
    // override this behaviour, for example to implement a continuous mode
    if (event->key() == Qt::Key_Escape) {
        quit();
        event->accept();
    }
}

void TechDrawHandler::mouseReleaseEvent(QMouseEvent* event)
{
    // the default behaviour is to quit - specific handler categories may
    // override this behaviour, for example to implement a continuous mode
    if (event->button() == Qt::RightButton) {
        quit();
        event->accept();
    }
}

void TechDrawHandler::quit()
{
    if (viewPage) {
        viewPage->deactivateHandler();
    }
}

QWidget* TechDrawHandler::getCursorWidget()
{
    return viewPage;
}

void TechDrawHandler::setWidgetCursor(QCursor cursor)
{
    if (viewPage) {
        viewPage->setCursor(cursor);
        viewPage->viewport()->setCursor(cursor);
    }
}

TechDraw::DrawPage* TechDrawHandler::getPage()
{
    return viewPage ? viewPage->getDrawPage() : nullptr;
}
