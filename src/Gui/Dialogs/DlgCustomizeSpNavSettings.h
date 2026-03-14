#ifndef DLGCUSTOMIZESPNAVSETTINGS_H
#define DLGCUSTOMIZESPNAVSETTINGS_H

#include "PropertyPage.h"
#include <Base/Parameter.h>
#include <memory>

class Ui_DlgCustomizeSpNavSettings;

namespace Gui
{
namespace Dialog
{
class DlgCustomizeSpNavSettings: public CustomizeActionPage
{
    Q_OBJECT

public:
    explicit DlgCustomizeSpNavSettings(QWidget* parent = nullptr);
    ~DlgCustomizeSpNavSettings() override;

protected Q_SLOTS:
    void onAddMacroAction(const QByteArray&) override;
    void onRemoveMacroAction(const QByteArray&) override;
    void onModifyMacroAction(const QByteArray&) override;

protected:
    void setupConnections();
    void on_CBDominant_clicked();
    void on_CBFlipYZ_clicked();
    void on_CBRotations_clicked();
    void on_CBTranslations_clicked();
    void on_SliderGlobal_sliderReleased();
    void on_CBEnablePanLR_clicked();
    void on_CBReversePanLR_clicked();
    void on_SliderPanLR_sliderReleased();
    void on_CBEnablePanUD_clicked();
    void on_CBReversePanUD_clicked();
    void on_SliderPanUD_sliderReleased();
    void on_CBEnableZoom_clicked();
    void on_CBReverseZoom_clicked();
    void on_SliderZoom_sliderReleased();
    void on_CBEnableTilt_clicked();
    void on_CBReverseTilt_clicked();
    void on_SliderTilt_sliderReleased();
    void on_CBEnableRoll_clicked();
    void on_CBReverseRoll_clicked();
    void on_SliderRoll_sliderReleased();
    void on_CBEnableSpin_clicked();
    void on_CBReverseSpin_clicked();
    void on_SliderSpin_sliderReleased();
    void on_ButtonDefaultSpNavMotions_clicked();
    void on_ButtonCalibrate_clicked();

protected:
    void changeEvent(QEvent* e) override;

private:
    ParameterGrp::handle spaceballMotionGroup() const;
    void setMessage(const QString& message);
    void initialize();

private:
    std::unique_ptr<Ui_DlgCustomizeSpNavSettings> ui;
    bool init;
};
}  // namespace Dialog
}  // namespace Gui

#endif  // DLGCUSTOMIZESPNAVSETTINGS_H
