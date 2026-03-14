#ifndef SKETCHERGUI_SketchMirrorDialog_H
#define SKETCHERGUI_SketchMirrorDialog_H

#include <QDialog>

#include <Mod/Sketcher/App/GeoEnum.h>


namespace SketcherGui
{

class Ui_SketchMirrorDialog;
class SketchMirrorDialog: public QDialog
{
    Q_OBJECT

public:
    SketchMirrorDialog();
    ~SketchMirrorDialog() override;

    int RefGeoid;
    Sketcher::PointPos RefPosid;

    void accept() override;

private:
    std::unique_ptr<Ui_SketchMirrorDialog> ui;
};

}  // namespace SketcherGui

#endif  // SKETCHERGUI_SketchMirrorDialog_H
