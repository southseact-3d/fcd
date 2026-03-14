#ifndef SKETCHERGUI_SketchRectangularArrayDialog_H
#define SKETCHERGUI_SketchRectangularArrayDialog_H

#include <QDialog>


namespace SketcherGui
{

class Ui_SketchRectangularArrayDialog;
class SketchRectangularArrayDialog: public QDialog
{
    Q_OBJECT

public:
    SketchRectangularArrayDialog();
    ~SketchRectangularArrayDialog() override;

    void accept() override;

    int Rows;
    int Cols;
    bool ConstraintSeparation;
    bool EqualVerticalHorizontalSpacing;
    bool Clone;

protected:
    void updateValues();

private:
    std::unique_ptr<Ui_SketchRectangularArrayDialog> ui;
};

}  // namespace SketcherGui

#endif  // SKETCHERGUI_SketchRectangularArrayDialog_H
