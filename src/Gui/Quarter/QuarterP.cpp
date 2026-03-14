#include "QuarterP.h"
#include "ImageReader.h"
#include "KeyboardP.h"
#include "SensorManager.h"


using namespace SIM::Coin3D::Quarter;
QuarterP::StateCursorMap * QuarterP::statecursormap = nullptr;

QuarterP::QuarterP()
{
  this->sensormanager = new SensorManager;
  this->imagereader = new ImageReader;
  assert(!QuarterP::statecursormap);
  QuarterP::statecursormap = new StateCursorMap;

}

QuarterP::~QuarterP()
{
  delete this->imagereader;
  delete this->sensormanager;

  assert(QuarterP::statecursormap);
  delete QuarterP::statecursormap;

  // FIXME: Why not use an atexit mechanism for this?
  if (KeyboardP::keyboardmap) {
    KeyboardP::keyboardmap->clear();
    KeyboardP::keypadmap->clear();
    delete KeyboardP::keyboardmap;
    delete KeyboardP::keypadmap;
    KeyboardP::keyboardmap = nullptr;
    KeyboardP::keypadmap = nullptr;
  }


}
