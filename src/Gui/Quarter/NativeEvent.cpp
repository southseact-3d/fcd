#include "NativeEvent.h"

using namespace SIM::Coin3D::Quarter;

#ifdef HAVE_SPACENAV_LIB
NativeEvent::NativeEvent(XEvent * nativeevent)
  : QEvent(QEvent::User)
{
  this->rawevent = nativeevent;
}


XEvent * 
NativeEvent::getEvent() const
{
  return static_cast<XEvent *>(this->rawevent);
}

#else // !HAVE_SPACENAV_LIB

// Dummy constructor when Spacenav is not available.
NativeEvent::NativeEvent()
  : QEvent(QEvent::User)
{
  this->rawevent = nullptr;
}

#endif // !HAVE_SPACENAV_LIB


NativeEvent::~NativeEvent()
{
#ifdef HAVE_SPACENAV_LIB
  delete (XEvent *) this->rawevent;
#endif // HAVE_SPACENAV_LIB
}

