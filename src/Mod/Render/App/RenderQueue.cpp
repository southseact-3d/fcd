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

#include "RenderQueue.h"

#include <algorithm>

namespace Render
{

RenderQueue& RenderQueue::instance()
{
    static RenderQueue s_instance;
    return s_instance;
}

RenderQueue::RenderQueue(QObject* parent)
    : QObject(parent)
{
    m_workerThread = QThread::create([this]() {
        while (m_running.load()) {
            // Find the next pending job
            RenderJob* nextJob = nullptr;
            {
                // Simple scan for the next pending job
                for (auto& job : m_jobs) {
                    if (job.status == RenderJobStatus::Pending) {
                        nextJob = &job;
                        break;
                    }
                }
            }

            if (!nextJob) {
                // No pending jobs, sleep briefly and check again
                QThread::msleep(100);
                continue;
            }

            // Mark as rendering
            nextJob->status = RenderJobStatus::Rendering;
            nextJob->startTime = QDateTime::currentDateTime();
            Q_EMIT jobStarted(nextJob->id);

            // Simulate render progress
            bool cancelled = false;
            for (int i = 0; i <= 100; i += 5) {
                if (!m_running.load()) {
                    cancelled = true;
                    break;
                }

                // Check if this specific job was cancelled
                if (nextJob->status == RenderJobStatus::Cancelled) {
                    cancelled = true;
                    break;
                }

                nextJob->progress = static_cast<float>(i) / 100.0F;
                Q_EMIT jobProgress(nextJob->id, nextJob->progress);
                QThread::msleep(50);
            }

            nextJob->endTime = QDateTime::currentDateTime();

            if (cancelled) {
                if (nextJob->status != RenderJobStatus::Cancelled) {
                    nextJob->status = RenderJobStatus::Failed;
                    Q_EMIT jobFailed(nextJob->id, QStringLiteral("Render cancelled"));
                }
                else {
                    Q_EMIT jobCancelled(nextJob->id);
                }
            }
            else {
                nextJob->status = RenderJobStatus::Completed;
                nextJob->progress = 1.0F;
                Q_EMIT jobCompleted(nextJob->id);
            }
        }
    });
}

RenderQueue::~RenderQueue()
{
    cancelAll();
    if (m_workerThread && m_workerThread->isRunning()) {
        m_running.store(false);
        m_workerThread->quit();
        m_workerThread->wait(3000);
    }
    delete m_workerThread;
}

int RenderQueue::addJob(const RenderSettings& settings, const QString& name)
{
    RenderJob job;
    job.id = m_nextId++;
    job.name = name;
    job.settings = settings;
    job.outputPath = settings.outputPath();
    job.status = RenderJobStatus::Pending;
    job.progress = 0.0F;

    m_jobs.push_back(job);
    Q_EMIT jobAdded(job.id);
    return job.id;
}

void RenderQueue::removeJob(int jobId)
{
    auto it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobId](const RenderJob& job) {
        return job.id == jobId;
    });

    if (it != m_jobs.end()) {
        if (it->status == RenderJobStatus::Rendering) {
            it->status = RenderJobStatus::Cancelled;
        }
        m_jobs.erase(it);
    }
}

void RenderQueue::startJob(int jobId)
{
    RenderJob* job = findJob(jobId);
    if (job && job->status == RenderJobStatus::Pending) {
        // Start the worker thread if not running
        if (m_workerThread && !m_workerThread->isRunning()) {
            m_running.store(true);
            m_workerThread->start();
        }
    }
}

void RenderQueue::cancelJob(int jobId)
{
    RenderJob* job = findJob(jobId);
    if (job) {
        if (job->status == RenderJobStatus::Rendering || job->status == RenderJobStatus::Pending) {
            job->status = RenderJobStatus::Cancelled;
            job->endTime = QDateTime::currentDateTime();
            Q_EMIT jobCancelled(jobId);
        }
    }
}

void RenderQueue::cancelAll()
{
    for (auto& job : m_jobs) {
        if (job.status == RenderJobStatus::Rendering || job.status == RenderJobStatus::Pending) {
            job.status = RenderJobStatus::Cancelled;
            job.endTime = QDateTime::currentDateTime();
            Q_EMIT jobCancelled(job.id);
        }
    }
}

void RenderQueue::clearCompleted()
{
    m_jobs.erase(
        std::remove_if(
            m_jobs.begin(),
            m_jobs.end(),
            [](const RenderJob& job) {
                return job.status == RenderJobStatus::Completed
                    || job.status == RenderJobStatus::Failed
                    || job.status == RenderJobStatus::Cancelled;
            }
        ),
        m_jobs.end()
    );
}

const std::vector<RenderJob>& RenderQueue::jobs() const
{
    return m_jobs;
}

RenderJob* RenderQueue::findJob(int jobId)
{
    auto it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobId](const RenderJob& job) {
        return job.id == jobId;
    });
    if (it != m_jobs.end()) {
        return &(*it);
    }
    return nullptr;
}

}  // namespace Render
