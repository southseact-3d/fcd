#include "FutureWatcherProgress.h"

using namespace Base;

FutureWatcherProgress::FutureWatcherProgress(const char* text, unsigned int steps)
    : seq(text, 100)
    , steps(steps)
{}

FutureWatcherProgress::~FutureWatcherProgress() = default;

void FutureWatcherProgress::progressValueChanged(int value)
{
    if (steps == 0) {
        return;
    }
    unsigned int step = (100 * static_cast<unsigned int>(value)) / steps;
    if (step > current) {
        current = step;
        seq.next();
    }
}

#include "moc_FutureWatcherProgress.cpp"
