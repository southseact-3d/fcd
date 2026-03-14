// The subsequent doxygen referenced page/subpages do not exist in the copy of Quarter used within FreeCad.
// To preserve the history and their origin the doxygen commands have been disabled but left in the file.
// /*!
//  \page examples More Examples
//
//  The examples code is included in Quarter and can be found in the
//  src/examples subdirectory.
//
//  \subpage directui
//
//  \subpage dynamicui
//
//  \subpage inheritui
//
//  \subpage mdi
//
//  \subpage examiner
//*/

#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>

#include "SensorManager.h"

#include "Quarter.h"
#include "QuarterP.h"


using namespace SIM::Coin3D::Quarter;

static QuarterP * self = nullptr;

/*!
  initialize Quarter, and implicitly Coin
 */
void
Quarter::init(bool initCoin)
{
  COMPILE_ONLY_BEFORE(2,0,0,"Should not be encapsulated in double Quarter namespace");
  if (self) {
    // FIXME: Use SoDebugError
    fprintf(stderr, "Quarter is already initialized\n");
    return;
  }

  if (initCoin) {
    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();
  }

  self = new QuarterP;
  self->initCoin = initCoin;

}

/*!
  clean up resources
 */
void
Quarter::clean()
{
  COMPILE_ONLY_BEFORE(2,0,0,"Should not be encapsulated in double Quarter namespace");
  assert(self);
  bool initCoin = self->initCoin;

  delete self;
  self = nullptr;

  if (initCoin) {
    // SoDB::finish() will clean up everything that has been
    // initialized. There's no need to add SoNodeKit::finish() and
    // SoInteraction::finish() like in TGS Inventor
    SoDB::finish();
  }
}

/*!
  override lower refresh rate limit
 */
void
Quarter::setTimerEpsilon(double sec)
{
  COMPILE_ONLY_BEFORE(2,0,0,"Should not be encapsulated in double Quarter namespace");
  if (!self) {
    fprintf(stderr, "Quarter is not initialized!\n");
    return;
  }

  self->sensormanager->setTimerEpsilon(sec);
}
