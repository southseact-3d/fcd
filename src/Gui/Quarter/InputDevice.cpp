#ifdef _MSC_VER
#pragma warning(disable : 4267)
#endif

#include <QInputEvent>
#include <Inventor/events/SoEvents.h>

#include "devices/InputDevice.h"


using namespace SIM::Coin3D::Quarter;

/*!
  \class SIM::Coin3D::Quarter::InputDevice InputDevice.h Quarter/devices/InputDevice.h

  \brief The InputDevice class is the base class for devices such as
  keyboard and mouse. It can be subclassed to support other devices.
*/

InputDevice::InputDevice(QuarterWidget* quarter) :
    quarter(quarter)
{
  this->mousepos = SbVec2s(0, 0);
}

/*!
  Sets the mouse position

  \param[in] pos position of mouse in pixel coordinates
*/
void
InputDevice::setMousePosition(const SbVec2s & pos)
{
  this->mousepos = pos;
}

/*!
  Sets the window size of the owning window

  \param[in] size in pixels
*/
void
InputDevice::setWindowSize(const SbVec2s & size)
{
  this->windowsize = size;
}

/*!
  Transforms a QEvent into an SoEvent

  \param[in,out] SoEvent the transformed event
  \param[in] QEvent incoming QEvent
*/
void
InputDevice::setModifiers(SoEvent * soevent, const QInputEvent * qevent)
{
  // FIXME: How do we get the time from the qevent? (20070306 frodo)
  soevent->setTime(SbTime::getTimeOfDay());

  // Note: On Mac OS X, the ControlModifier value corresponds to the
  // Command keys on the Macintosh keyboard, and the MetaModifier
  // value corresponds to the Control keys.
  soevent->setShiftDown(qevent->modifiers() & Qt::ShiftModifier);
  soevent->setAltDown(qevent->modifiers() & Qt::AltModifier);
  soevent->setCtrlDown(qevent->modifiers() & Qt::ControlModifier);
}

/*!
  \var InputDevice::mousepos

  Holds the last known position of the mouse. This should be set even
  for a keyboard event.
*/

/*!
  \var InputDevice::windowsize

  Holds the size of the owning window
*/

#undef PRIVATE
