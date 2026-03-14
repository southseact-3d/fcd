#ifndef SKETCHERGUI_EditDatumDialog_H
#define SKETCHERGUI_EditDatumDialog_H

#include <QObject>
#include <memory>


namespace Sketcher
{
class Constraint;
class SketchObject;
}  // namespace Sketcher

namespace SketcherGui
{
class ViewProviderSketch;
class Ui_InsertDatum;

bool checkConstraintName(const Sketcher::SketchObject* sketch, std::string constraintName);

class EditDatumDialog: public QObject
{
    Q_OBJECT

public:
    EditDatumDialog(ViewProviderSketch* vp, int ConstrNbr);
    EditDatumDialog(Sketcher::SketchObject* pcSketch, int ConstrNbr);
    ~EditDatumDialog() override;

    int exec(bool atCursor = true);
    bool isSuccess();

private:
    Sketcher::SketchObject* sketch;
    Sketcher::Constraint* Constr;
    int ConstrNbr;
    bool success;
    std::unique_ptr<Ui_InsertDatum> ui_ins_datum;

private Q_SLOTS:
    void accepted();
    void rejected();
    void drivingToggled(bool);
    void datumChanged();
    void formEditorOpened(bool);
    void typeChanged(bool);

private:
    void performAutoScale(double newDatum);
};

}  // namespace SketcherGui
#endif  // SKETCHERGUI_DrawSketchHandler_H
