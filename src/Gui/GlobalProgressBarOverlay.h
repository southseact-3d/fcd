#ifndef GUI_GLOBALPROGRESSBAROVERLAY_H
#define GUI_GLOBALPROGRESSBAROVERLAY_H

#include "FCGlobal.h"

class QProgressBar;
class QWidget;

namespace Gui
{
class GuiExport GlobalProgressBarOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalProgressBarOverlay(QWidget* parent = nullptr);
    ~GlobalProgressBarOverlay() override;

    /** Show the overlay */
    void showOverlay();
    /** Hide the overlay */
    void hideOverlay();
    /** Set the range of the progress bar */
    void setRange(int minimum, int maximum);
    /** Set the current progress value */
    void setValue(int value);
    /** Reset the progress bar */
    void reset();

private:
    QProgressBar* progressBar;
} // namespace Gui

#endif // GUI_GLOBALPROGRESSBAROVERLAY_H