#include "App/DocumentObserver.h"


#include <QObject>
#include <QSet>

#include <Mod/Part/App/PreviewExtension.h>

namespace PartGui
{

/**
 * Qt-based implementation of the PreviewUpdateScheduler.
 *
 * This implementation uses the Qt Event Loop to debounce recompute requests.
 * Requests are queued and a flush is triggered via a queued connection,
 * ensuring that the actual update happens once the control returns to the
 * event loop after all pending property changes are processed.
 */
class QtPreviewUpdateScheduler final: public QObject, public Part::PreviewUpdateScheduler
{
    Q_OBJECT

public:
    explicit QtPreviewUpdateScheduler(QObject* parent = nullptr);

    /**
     * Schedules a preview recompute using the Qt Event Loop.
     *
     * Adds the object to a unique set to avoid duplicate work and schedules
     * a call to flush() using Qt::QueuedConnection if one isn't already pending.
     */
    void schedulePreviewRecompute(App::DocumentObject* object) override;

private Q_SLOTS:
    void flush();

private:
    std::unordered_set<App::DocumentObjectWeakPtrT> toBeUpdated;
    bool scheduled = false;
};

}  // namespace PartGui

#endif  // FREECAD_PREVIEWUPDATESCHEDULER_H
