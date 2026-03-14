#include "PreviewUpdateScheduler.h"

using namespace PartGui;

QtPreviewUpdateScheduler::QtPreviewUpdateScheduler(QObject* parent)
    : QObject(parent)
{}

inline void QtPreviewUpdateScheduler::schedulePreviewRecompute(App::DocumentObject* object)
{
    if (!object) {
        return;
    }

    toBeUpdated.emplace(object);

    // if method call was already scheduled there is no need to queue another one
    if (scheduled) {
        return;
    }

    QMetaObject::invokeMethod(this, &QtPreviewUpdateScheduler::flush, Qt::QueuedConnection);
}

void QtPreviewUpdateScheduler::flush()
{
    scheduled = false;

    // use std::exchange to prevent race conditions on updates that could occur during a flush
    for (auto objects = std::exchange(this->toBeUpdated, {}); auto& object : objects) {
        if (object.expired()) {
            continue;
        }

        if (auto* previewExtension = object->getExtensionByType<Part::PreviewExtension>(true)) {
            previewExtension->updatePreview();
        }
    }
}

#include "moc_PreviewUpdateScheduler.cpp"
