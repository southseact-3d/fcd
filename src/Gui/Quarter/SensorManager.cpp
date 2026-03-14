#include "SensorManager.h"

#include <QTimer>

#include <Inventor/SbTime.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoRenderManager.h>
#include <Inventor/C/threads/thread.h>

#include "SignalThread.h"


using namespace SIM::Coin3D::Quarter;

SensorManager::SensorManager()
  : inherited()
{
  this->mainthreadid = cc_thread_id();
  this->signalthread = new SignalThread();

  QObject::connect(this->signalthread, &SignalThread::triggerSignal,
                   this, &SensorManager::sensorQueueChanged);

  this->idletimer = new QTimer;
  this->delaytimer = new QTimer;
  this->timerqueuetimer = new QTimer;

  this->idletimer->setSingleShot(true);
  this->delaytimer->setSingleShot(true);
  this->timerqueuetimer->setSingleShot(true);

  this->connect(this->idletimer, &QTimer::timeout, this, &SensorManager::idleTimeout);
  this->connect(this->delaytimer, &QTimer::timeout, this, &SensorManager::delayTimeout);
  this->connect(this->timerqueuetimer, &QTimer::timeout, this, &SensorManager::timerQueueTimeout);

  SoDB::getSensorManager()->setChangedCallback(SensorManager::sensorQueueChangedCB, this);
  this->timerEpsilon = 1.0 / 5000.0;

  SoDB::setRealTimeInterval(1.0 / 25.0);
  SoRenderManager::enableRealTimeUpdate(false);
}

SensorManager::~SensorManager()
{
  // remove the Coin callback before shutting down
  SoDB::getSensorManager()->setChangedCallback(nullptr, nullptr);

  if (this->signalthread->isRunning()) {
    this->signalthread->stopThread();
    this->signalthread->wait();
  }
  delete this->signalthread;
  delete this->idletimer;
  delete this->delaytimer;
  delete this->timerqueuetimer;
}

void
SensorManager::sensorQueueChangedCB(void * closure)
{
  SensorManager * thisp = (SensorManager * ) closure;

  // if we get a callback from another thread, route the callback
  // through SignalThread so that we receive the callback in the
  // QApplication thread (needed since QTimer isn't thread safe)
  if (cc_thread_id() != thisp->mainthreadid) {
    if (!thisp->signalthread->isRunning()) thisp->signalthread->start();
    thisp->signalthread->trigger();
  }
  else {
    thisp->sensorQueueChanged();
  }
}

void
SensorManager::sensorQueueChanged()
{
  SoSensorManager * sensormanager = SoDB::getSensorManager();
  assert(sensormanager);

  SbTime interval;
  if (sensormanager->isTimerSensorPending(interval)) {
    interval -= SbTime::getTimeOfDay();
    if (interval.getValue() < this->timerEpsilon) {
      interval.setValue(this->timerEpsilon);
    }
    if (!this->timerqueuetimer->isActive()) {
      this->timerqueuetimer->start(interval.getMsecValue());
    } else {
      this->timerqueuetimer->setInterval(interval.getMsecValue());
    }
  } else if (this->timerqueuetimer->isActive()) {
    this->timerqueuetimer->stop();
  }

  if (sensormanager->isDelaySensorPending()) {
    this->idletimer->start(0);

    if (!this->delaytimer->isActive()) {
      SbTime time = SoDB::getDelaySensorTimeout();
      if (time != SbTime::zero()) {
        this->delaytimer->start(time.getMsecValue());
      }
    }
  } else {
    if (this->idletimer->isActive()) {
      this->idletimer->stop();
    }
    if (this->delaytimer->isActive()) {
      this->delaytimer->stop();
    }
  }
}

void
SensorManager::idleTimeout()
{
  SoDB::getSensorManager()->processTimerQueue();
  SoDB::getSensorManager()->processDelayQueue(true);
  this->sensorQueueChanged();
}

void
SensorManager::timerQueueTimeout()
{
  SoDB::getSensorManager()->processTimerQueue();
  this->sensorQueueChanged();
}

void
SensorManager::delayTimeout()
{
  SoDB::getSensorManager()->processTimerQueue();
  SoDB::getSensorManager()->processDelayQueue(false);
  this->sensorQueueChanged();
}

void
SensorManager::setTimerEpsilon(double sec)
{
  this->timerEpsilon = sec;
}
