#ifndef GUI_GLOBALPROGRESSBAROVERLAY_H
#define GUI_GLOBALPROGRESSBAROVERLAY_H

#include <FCGlobal.h>
#include <QWidget>
#include <QProgressBar>

namespace Gui
{
class GuiExport GlobalProgressBarOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalProgressBarOverlay(QWidget* parent = nullptr);
    ~GlobalProgressBarOverlay() override;

    void showOverlay();
    void hideOverlay();
    void setRange(int minimum, int maximum);
    void setValue(int value);
    void reset();

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void updatePosition();

    QProgressBar* progressBar;
};

} // namespace Gui

#endif // GUI_GLOBALPROGRESSBAROVERLAY_H