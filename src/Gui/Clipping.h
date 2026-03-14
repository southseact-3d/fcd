#ifndef GUI_DIALOG_CLIPPING_H
#define GUI_DIALOG_CLIPPING_H

#include <QDialog>
#include <FCGlobal.h>

namespace Gui
{
class View3DInventor;
namespace Dialog
{

/**
 * @author Werner Mayer
 */
class GuiExport Clipping: public QDialog
{
    Q_OBJECT

public:
    static Clipping* makeDockWidget(Gui::View3DInventor*);
    Clipping(Gui::View3DInventor* view, QWidget* parent = nullptr);
    ~Clipping() override;

protected:
    void setupConnections();
    void onGroupBoxXToggled(bool);
    void onGroupBoxYToggled(bool);
    void onGroupBoxZToggled(bool);
    void onClipXValueChanged(double);
    void onClipYValueChanged(double);
    void onClipZValueChanged(double);
    void onFlipClipXClicked();
    void onFlipClipYClicked();
    void onFlipClipZClicked();
    void onGroupBoxViewToggled(bool);
    void onClipViewValueChanged(double);
    void onFromViewClicked();
    void onAdjustViewdirectionToggled(bool);
    void onDirXValueChanged(double);
    void onDirYValueChanged(double);
    void onDirZValueChanged(double);

public:
    void reject() override;

private:
    class Private;
    Private* d;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_CLIPPING_H
