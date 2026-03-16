// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef PARTDESIGN_WORKBENCH_MODE_HANDLER_H
#define PARTDESIGN_WORKBENCH_MODE_HANDLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace PartDesignGui
{

enum class WorkbenchMode
{
    DesignMode,
    MeshMode,
    MaterialMode
};

class WorkbenchModeHandler: public QObject
{
    Q_OBJECT

public:
    explicit WorkbenchModeHandler(QObject* parent = nullptr);
    ~WorkbenchModeHandler() override;

    WorkbenchMode currentMode() const;
    void switchToMode(WorkbenchMode mode);
    void saveState() const;
    void restoreState();

    static const QString& modeToString(WorkbenchMode mode);
    static WorkbenchMode stringToMode(const QString& str);

Q_SIGNALS:
    void modeChanged(WorkbenchMode mode);

private:
    void saveStateForMode(WorkbenchMode mode);
    void restoreStateForMode(WorkbenchMode mode);

    WorkbenchMode currentMode_;
    bool stateSaved;
};

inline WorkbenchMode WorkbenchModeHandler::currentMode() const
{
    return currentMode_;
}

}  // namespace PartDesignGui

#endif  // PARTDESIGN_WORKBENCH_MODE_HANDLER_H
