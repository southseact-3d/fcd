#ifndef GUI_DIALOG_DlgActivateWindowImp_H
#define GUI_DIALOG_DlgActivateWindowImp_H

#include <QDialog>
#include <memory>
#include <Base/Quantity.h>
#include <Base/Unit.h>

namespace Gui
{
namespace Dialog
{
class Ui_DlgUnitCalculator;

/**
 * The DlgUnitsCalculator provides a unit conversion dialog
 * \author Juergen Riegel
 */
class DlgUnitsCalculator: public QDialog
{
    Q_OBJECT

public:
    explicit DlgUnitsCalculator(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgUnitsCalculator() override;

protected:
    void accept() override;
    void reject() override;

protected:
    void textChanged(const QString);
    void valueChanged(const Base::Quantity&);
    void onUnitsBoxActivated(int);
    void onComboBoxSchemeActivated(int);
    void onSpinBoxDecimalsValueChanged(int);

    void copy();
    void returnPressed();

    void parseError(const QString& errorText);

private:
    Base::Quantity actValue;
    std::unique_ptr<Ui_DlgUnitCalculator> ui;
    QList<Base::Unit> units;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DlgActivateWindowImp_H
