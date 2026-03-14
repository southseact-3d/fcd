#ifndef DRAWINGGUI_DLGTEMPLATEFIELD_H
#define DRAWINGGUI_DLGTEMPLATEFIELD_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <memory>
#include <QDialog>
#include <QString>


namespace TechDrawGui {

class Ui_dlgTemplateField;
class DlgTemplateField : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTemplateField( QWidget *parent = nullptr );
    ~DlgTemplateField() override = default;

    void setFieldName(std::string name);
    void setFieldLength(int length);
    void setFieldContent(std::string content);
    void setAutofillContent(std::string content);
    QString getFieldContent();
    bool getAutofillState();

public Q_SLOTS:
    void accept() override;
    void reject() override;

protected:
    void changeEvent(QEvent *e) override;

private:
    std::shared_ptr<Ui_dlgTemplateField> ui;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_DLGTEMPLATEFIELD_H
