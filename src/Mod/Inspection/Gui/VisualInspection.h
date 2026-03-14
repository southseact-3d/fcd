#ifndef INSPECTIONGUI_VISUALINSPECTION_H
#define INSPECTIONGUI_VISUALINSPECTION_H

#include <QDialog>


class QTreeWidgetItem;
class QPushButton;

namespace InspectionGui
{
class Ui_VisualInspection;
class VisualInspection: public QDialog
{
    Q_OBJECT

public:
    explicit VisualInspection(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~VisualInspection() override;

    void accept() override;

protected Q_SLOTS:
    void onActivateItem(QTreeWidgetItem*);
    void loadSettings();
    void saveSettings();

private:
    Ui_VisualInspection* ui;
    QPushButton* buttonOk;
};

}  // namespace InspectionGui

#endif  // INSPECTIONGUI_VISUALINSPECTION_H
