#ifndef PATHSIMULATOR_VIEWCAMSIMULATOR_H
#define PATHSIMULATOR_VIEWCAMSIMULATOR_H

#include <Gui/MDIView.h>

namespace CAMSimulator
{
class DlgCAMSimulator;

class ViewCAMSimulator: public Gui::MDIView
{
public:
    ViewCAMSimulator(
        Gui::Document* pcDocument,
        QWidget* parent,
        Qt::WindowFlags wflags = Qt::WindowFlags()
    );

    ViewCAMSimulator* clone() override;

    DlgCAMSimulator& dlg();

protected:
    DlgCAMSimulator* mDlg;
};

}  // namespace CAMSimulator

#endif /* PATHSIMULATOR_VIEWCAMSIMULATOR_H */
