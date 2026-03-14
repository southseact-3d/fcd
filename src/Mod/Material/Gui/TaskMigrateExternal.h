#ifndef MATGUI_TASKMIGRATEEXTERNAL_H
#define MATGUI_TASKMIGRATEEXTERNAL_H

#include <memory>

#include <QPushButton>

#include <Gui/TaskView/TaskDialog.h>

#include <Mod/Material/App/ModelManager.h>
#include <Mod/Material/App/MaterialManager.h>

namespace MatGui {

class Ui_TaskMigrateExternal;

class DlgMigrateExternal: public QWidget
{
    Q_OBJECT

public:
    explicit DlgMigrateExternal(QWidget* parent = nullptr);
    ~DlgMigrateExternal() override = default;
    void migrate();
    void statusUpdate(const QString& status);

private:
    void showLibraries();

    std::shared_ptr<Ui_TaskMigrateExternal> ui;
};

class TaskMigrateExternal: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskMigrateExternal();

public:
    bool accept() override;
    bool reject() override;
    QDialogButtonBox::StandardButtons getStandardButtons() const override;
    void modifyStandardButtons(QDialogButtonBox*) override;
    void onMigrate(bool checked);

private:
    DlgMigrateExternal* _widget;
    QPushButton* _migrateButton;
    QPushButton* _closeButton;
};

} // namespace MatGui

#endif  // MATGUI_TASKMIGRATEEXTERNAL_H
