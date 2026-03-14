#ifndef MESHGUI_DLG_EVALUATE_SETTINGS_H
#define MESHGUI_DLG_EVALUATE_SETTINGS_H

#include <FCGlobal.h>

#include <QDialog>

namespace MeshGui
{

class Ui_DlgEvaluateSettings;

/**
 * \author Werner Mayer
 */
class DlgEvaluateSettings: public QDialog
{
    Q_OBJECT

public:
    explicit DlgEvaluateSettings(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgEvaluateSettings() override;

    void setNonmanifoldPointsChecked(bool);
    bool isNonmanifoldPointsChecked() const;

    void setFoldsChecked(bool);
    bool isFoldsChecked() const;

    void setDegeneratedFacetsChecked(bool);
    bool isDegeneratedFacetsChecked() const;

private:
    Ui_DlgEvaluateSettings* ui;

    Q_DISABLE_COPY_MOVE(DlgEvaluateSettings)
};

}  // namespace MeshGui

#endif  // MESHGUI_DLG_EVALUATE_SETTINGS_H
