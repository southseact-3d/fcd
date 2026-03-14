#ifndef GUI_TEXTUREMAPPING_H
#define GUI_TEXTUREMAPPING_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <QDialog>

class SoGroup;
class SoTexture2;
class SoTextureCoordinateEnvironment;

namespace Gui
{
namespace Dialog
{

class Ui_TextureMapping;
class GuiExport TextureMapping: public QDialog
{
    Q_OBJECT

public:
    explicit TextureMapping(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~TextureMapping() override;
    void accept() override;
    void reject() override;

private:
    void onFileChooserFileNameSelected(const QString&);
    void onCheckEnvToggled(bool);

protected:
    void changeEvent(QEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    SoGroup* grp;
    SoTexture2* tex;
    SoTextureCoordinateEnvironment* env;
    QString fileName;
    Ui_TextureMapping* ui;
};

class TaskTextureMapping: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskTextureMapping();
    ~TaskTextureMapping() override;

public:
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Close;
    }

private:
    TextureMapping* dialog;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_TEXTUREMAPPING_H
