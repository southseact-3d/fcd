#ifndef GUI_DIALOG_DLGSETTINGSCOLORGRADIENT_IMP_H
#define GUI_DIALOG_DLGSETTINGSCOLORGRADIENT_IMP_H

#include <App/ColorModel.h>
#include <QDialog>
#include <memory>

class QDoubleValidator;

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsColorGradient;

/**
 * The DlgSettingsColorGradientImp class implements a preference page to change settings
 * for the color gradient bar.
 * @author Werner Mayer
 */
class DlgSettingsColorGradientImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgSettingsColorGradientImp(
        const App::ColorGradient& cg,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~DlgSettingsColorGradientImp() override;

    void accept() override;

    /** @name Color profile */
    //@{
    App::ColorGradientProfile getProfile() const;
    void setProfile(const App::ColorGradientProfile& pro);
    //@}
    /** @name Parameter range and scale */
    //@{
    void setNumberOfDecimals(int, float fMin, float fMax);
    int numberOfDecimals() const;
    //@}

private:
    void setupConnections();

    /** @name Color model */
    //@{
    void setColorModelNames(const std::vector<std::string>&);
    void setColorModel(std::size_t tModel);
    std::size_t colorModel() const;
    //@}
    /** @name Color style */
    //@{
    void setColorStyle(App::ColorBarStyle tStyle);
    App::ColorBarStyle colorStyle() const;
    //@}
    /** @name Display mode */
    //@{
    void setOutGrayed(bool grayed);
    bool isOutGrayed() const;
    void setOutInvisible(bool invisible);
    bool isOutInvisible() const;
    //@}
    /** @name Parameter range and scale */
    //@{
    void setRange(float fMin, float fMax);
    void getRange(float& fMin, float& fMax) const;
    void setNumberOfLabels(int);
    int numberOfLabels() const;
    //@}

Q_SIGNALS:
    void colorModelChanged();

private:
    QDoubleValidator* validator;
    std::unique_ptr<Ui_DlgSettingsColorGradient> ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSCOLORGRADIENT_IMP_H
