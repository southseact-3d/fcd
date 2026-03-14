#ifndef GUI_DIALOG_DEMOMODE_H
#define GUI_DIALOG_DEMOMODE_H

#include <Inventor/SbVec3f.h>
#include <QDialog>
#include <FCGlobal.h>


class QTimer;
class SoCamera;
class SbVec3f;
class SbRotation;

namespace Gui
{
class View3DInventor;
namespace Dialog
{

/**
 * @author Werner Mayer
 */
class Ui_DemoMode;
class GuiExport DemoMode: public QDialog
{
    Q_OBJECT

public:
    explicit DemoMode(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DemoMode() override;

    void accept() override;
    void reject() override;

protected:
    void setupConnections();
    void onPlayButtonToggled(bool);
    void onFullscreenToggled(bool);
    void onTimerCheckToggled(bool);
    void onSpeedSliderValueChanged(int);
    void onAngleSliderValueChanged(int);
    void onTimeoutValueChanged(int);
    void onAutoPlay();

private:
    void reset();
    float getSpeed(int) const;
    void reorientCamera(SoCamera* cam, const SbRotation& rot);
    SbVec3f getDirection(Gui::View3DInventor*) const;
    Gui::View3DInventor* activeView() const;
    void startAnimation(Gui::View3DInventor*);
    void changeEvent(QEvent* e) override;
    bool eventFilter(QObject*, QEvent*) override;
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;

private:
    int oldvalue;
    SbVec3f viewAxis;
    bool wasHidden;
    QPoint pnt;
    QPoint oldPos;
    Ui_DemoMode* ui;
    QTimer* timer;
    QTimer* showHideTimer;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DEMOMODE_H
