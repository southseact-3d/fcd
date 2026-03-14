#ifndef SKETCHERGUI_SketcherRegularPolygonDialog_H
#define SKETCHERGUI_SketcherRegularPolygonDialog_H

#include <QDialog>


namespace SketcherGui
{

class Ui_SketcherRegularPolygonDialog;
class SketcherRegularPolygonDialog: public QDialog
{
    Q_OBJECT

public:
    SketcherRegularPolygonDialog();
    ~SketcherRegularPolygonDialog() override;

    void accept() override;

    int sides;

protected:
    void updateValues();

private:
    std::unique_ptr<Ui_SketcherRegularPolygonDialog> ui;
};

}  // namespace SketcherGui

#endif  // SKETCHERGUI_SketcherRegularPolygonDialog_H
