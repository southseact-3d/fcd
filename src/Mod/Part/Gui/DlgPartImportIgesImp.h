#ifndef DlgPartImportIgesImp_H
#define DlgPartImportIgesImp_H

#include <QDialog>
#include <memory>

namespace PartGui
{
class Ui_DlgPartImportIges;
class DlgPartImportIgesImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgPartImportIgesImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgPartImportIgesImp() override;

public Q_SLOTS:
    virtual void OnApply();
    virtual void onChooseFileName();

private:
    std::unique_ptr<Ui_DlgPartImportIges> ui;
};

}  // namespace PartGui

#endif  // DlgPartImportIgesImp_H
