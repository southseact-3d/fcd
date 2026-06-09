// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD Contributors                          *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#ifndef RENDER_RENDERQUEUE_H
#define RENDER_RENDERQUEUE_H

#include <atomic>
#include <vector>

#include <QObject>
#include <QThread>
#include <QDateTime>
#include <QString>

#include "RenderSettings.h"
#include "RenderAppGlobal.h"

namespace Render
{

enum class RenderJobStatus
{
    Pending,
    Rendering,
    Completed,
    Failed,
    Cancelled
};

struct RenderJob
{
    int id;
    QString name;
    QString outputPath;
    RenderSettings settings;
    RenderJobStatus status;
    float progress{0.0};
    QDateTime startTime;
    QDateTime endTime;
};

class RenderAppExport RenderQueue : public QObject
{
    Q_OBJECT

public:
    static RenderQueue& instance();

    int addJob(const RenderSettings& settings, const QString& name);
    void removeJob(int jobId);
    void startJob(int jobId);
    void cancelJob(int jobId);
    void cancelAll();
    void clearCompleted();

    const std::vector<RenderJob>& jobs() const;

Q_SIGNALS:
    void jobAdded(int jobId);
    void jobStarted(int jobId);
    void jobProgress(int jobId, float progress);
    void jobCompleted(int jobId);
    void jobFailed(int jobId, const QString& error);
    void jobCancelled(int jobId);

private:
    explicit RenderQueue(QObject* parent = nullptr);
    ~RenderQueue() override;
    Q_DISABLE_COPY_MOVE(RenderQueue)

    std::vector<RenderJob> m_jobs;
    int m_nextId{1};
    QThread* m_workerThread{nullptr};
    std::atomic<bool> m_running{false};

    RenderJob* findJob(int jobId);
};

}  // namespace Render

#endif  // RENDER_RENDERQUEUE_H
