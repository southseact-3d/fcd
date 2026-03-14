#ifndef BASE_FUTUREWATCHER_H
#define BASE_FUTUREWATCHER_H

#include <QObject>
#include <Base/Sequencer.h>


namespace Base
{

class BaseExport FutureWatcherProgress: public QObject
{
    Q_OBJECT

public:
    FutureWatcherProgress(const char* text, unsigned int steps);
    ~FutureWatcherProgress() override;

    FutureWatcherProgress(const FutureWatcherProgress&) = delete;
    FutureWatcherProgress(FutureWatcherProgress&&) = delete;
    FutureWatcherProgress& operator=(const FutureWatcherProgress&) = delete;
    FutureWatcherProgress& operator=(FutureWatcherProgress&&) = delete;

public Q_SLOTS:
    void progressValueChanged(int value);

private:
    Base::SequencerLauncher seq;
    unsigned int steps, current {0};
};
}  // namespace Base

#endif  // BASE_FUTUREWATCHER_H
