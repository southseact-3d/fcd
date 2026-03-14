#ifndef GUI_DIALOG_DLGINPUTDIALOGIMP_H
#define GUI_DIALOG_DLGINPUTDIALOGIMP_H

#include <memory>
#include <QDialog>
#include <FCGlobal.h>

class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QComboBox;

namespace Gui
{
class UIntSpinBox;

namespace Dialog
{
class Ui_DlgInputDialog;

/**
 * The DlgInputDialogImp dialog class does basically the same as Qt's QInputDialog
 * unless that it provides no static function but the application programmer must
 * create an instance and prepare it. This requires a little more work but increases
 * the flexibility.
 * \author Werner Mayer
 */
class GuiExport DlgInputDialogImp: public QDialog
{
    Q_OBJECT

public:
    enum Type
    {
        LineEdit,
        SpinBox,
        UIntBox,
        FloatSpinBox,
        ComboBox
    };

    DlgInputDialogImp(const QString& label, QWidget* parent = nullptr, bool modal = true, Type = LineEdit);
    ~DlgInputDialogImp() override;

    void setType(Type t);
    Type type() const;

    QSpinBox* getSpinBox() const;
    Gui::UIntSpinBox* getUIntBox() const;
    QDoubleSpinBox* getFloatSpinBox() const;
    QLineEdit* getLineEdit() const;
    QComboBox* getComboBox() const;

protected Q_SLOTS:
    void textChanged(const QString& s);
    void tryAccept();

protected:
    Type inputtype;

private:
    std::unique_ptr<Ui_DlgInputDialog> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGINPUTDIALOGIMP_H
