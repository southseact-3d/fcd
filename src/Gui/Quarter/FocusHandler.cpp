#include <QEvent>
#include <Inventor/SoEventManager.h>
#include <Inventor/scxml/SoScXMLStateMachine.h>

#include "QuarterWidget.h"
#include "eventhandlers/FocusHandler.h"


using namespace SIM::Coin3D::Quarter;

FocusHandler::FocusHandler(QObject * parent)
  : QObject(parent)
{
  this->quarterwidget = dynamic_cast<QuarterWidget *>(parent);
}

FocusHandler::~FocusHandler()
{

}

bool 
FocusHandler::eventFilter(QObject * obj, QEvent * event)
{
  switch (event->type()) {
  case QEvent::FocusIn:
    this->focusEvent("sim.coin3d.coin.InputFocus.IN");
    break;
  case QEvent::FocusOut:
    this->focusEvent("sim.coin3d.coin.InputFocus.OUT");
    break;
  default:
    break;
  }
  return QObject::eventFilter(obj, event);
}

void
FocusHandler::focusEvent(const SbName & focusevent)
{
  SoEventManager * eventmanager = this->quarterwidget->getSoEventManager();
  for (int c = 0; c < eventmanager->getNumSoScXMLStateMachines(); ++c) {
    SoScXMLStateMachine * sostatemachine =
      eventmanager->getSoScXMLStateMachine(c);
    if (sostatemachine->isActive()) {
      sostatemachine->queueEvent(focusevent);
      sostatemachine->processEventQueue();
    }
  }
}
