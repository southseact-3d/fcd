#ifndef POINTSGUI_DLGREADPOINTS_H
#define POINTSGUI_DLGREADPOINTS_H

#include <FCGlobal.h>

#include <QDialog>
#include <memory>


namespace PointsGui
{
class Ui_DlgPointsRead;

/** The points read dialog
 */
class DlgPointsReadImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgPointsReadImp(
        const char* FileName,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~DlgPointsReadImp() override;

private:
    std::unique_ptr<Ui_DlgPointsRead> ui;
    std::string _FileName;

    Q_DISABLE_COPY_MOVE(DlgPointsReadImp)
};

}  // namespace PointsGui

#endif  // POINTSGUI_DLGREADPOINTS_H
