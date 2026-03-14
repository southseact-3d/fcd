#ifndef SKETCHERGUI_SketchOrientationDialog_H
#define SKETCHERGUI_SketchOrientationDialog_H

#include <QDialog>

#include <Base/Placement.h>
#include <Mod/Sketcher/SketcherGlobal.h>


namespace SketcherGui
{

class Ui_SketchOrientationDialog;
class SketcherGuiExport SketchOrientationDialog: public QDialog
{
    Q_OBJECT

public:
    SketchOrientationDialog();
    ~SketchOrientationDialog() override;

    Base::Placement Pos;
    int DirType;

    void accept() override;

protected Q_SLOTS:
    void onPreview();

private:
    std::unique_ptr<Ui_SketchOrientationDialog> ui;
};

}  // namespace SketcherGui

#endif  // SKETCHERGUI_SketchOrientationDialog_H
