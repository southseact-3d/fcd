#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

#include <QEvent>
#include <QKeyEvent>
#include <Inventor/events/SoEvent.h>

#include "KeyboardP.h"
#include "devices/Keyboard.h"


using namespace SIM::Coin3D::Quarter;

#define PRIVATE(obj) obj->pimpl

Keyboard::Keyboard(QuarterWidget* quarter) :
  InputDevice(quarter)
{
  PRIVATE(this) = new KeyboardP(this);
}

Keyboard::~Keyboard()
{
  delete PRIVATE(this);
}

/*! Translates from QKeyEvents to SoKeyboardEvents
 */
const SoEvent *
Keyboard::translateEvent(QEvent * event)
{
  switch (event->type()) {
  case QEvent::KeyPress:
  case QEvent::KeyRelease:
    return PRIVATE(this)->keyEvent((QKeyEvent *) event);
  default:
    return nullptr;
  }
}

#undef PRIVATE
