#ifndef GUIRAWNATIVEINPUTEVENTFILTER_H
#define GUIRAWNATIVEINPUTEVENTFILTER_H

#include <QAbstractNativeEventFilter>

namespace Gui
{
class RawInputEventFilter: public QAbstractNativeEventFilter
{
public:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    using EventFilter = bool (*)(void* message, long* result);
#else
    using EventFilter = bool (*)(void* message, qintptr* result);
#endif
    RawInputEventFilter(EventFilter filter)
        : eventFilter(filter)
    {}
    virtual ~RawInputEventFilter()
    {}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEventFilter(const QByteArray& /*eventType*/, void* message, long* result)
    {
        return eventFilter(message, result);
    }
#else
    virtual bool nativeEventFilter(const QByteArray& /*eventType*/, void* message, qintptr* result)
    {
        return eventFilter(message, result);
    }
#endif

private:
    EventFilter eventFilter;
};
}  // namespace Gui

#endif  // GUIRAWNATIVEINPUTEVENTFILTER_H
