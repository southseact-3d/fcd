#include <QMutexLocker>

#include "SignalThread.h"

using namespace SIM::Coin3D::Quarter;

SignalThread::SignalThread()
  : isstopped(false)
{
}

SignalThread::~SignalThread()
{
}

void
SignalThread::trigger()
{
  // lock first to make sure the QThread is actually waiting for a signal
  QMutexLocker ml(&this->mutex);
  this->waitcond.wakeOne();
}

void
SignalThread::stopThread()
{
  QMutexLocker ml(&this->mutex);
  this->isstopped = true;
  this->waitcond.wakeOne();
}


void
SignalThread::run()
{
  QMutexLocker ml(&this->mutex);
  while (!this->isstopped) {
    // just wait, and trigger every time we receive a signal
    this->waitcond.wait(&this->mutex);
    if (!this->isstopped) {
      Q_EMIT triggerSignal();
    }
  }
}
