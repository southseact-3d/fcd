#ifndef DlgPartImportStepImp_H
#define DlgPartImportStepImp_H

#include <QDialog>
#include <memory>

namespace PartGui
{
class Ui_DlgPartImportStep;

class DlgPartImportStepImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgPartImportStepImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgPartImportStepImp() override;

public Q_SLOTS:
    virtual void OnApply();
    virtual void onChooseFileName();

private:
    std::unique_ptr<Ui_DlgPartImportStep> ui;
};

}  // namespace PartGui

#endif  // DlgPartImportStepImp_H
