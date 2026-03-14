#ifndef GUI_DIALOG_DLGMATERIALPROPERTIES_IMP_H
#define GUI_DIALOG_DLGMATERIALPROPERTIES_IMP_H

#include <QDialog>
#include <memory>
#include <vector>
#include <App/Material.h>

namespace Base
{
class Color;
}

namespace App
{
class Material;
}

namespace Gui
{
class ViewProvider;

namespace Dialog
{
class Ui_DlgMaterialProperties;

class GuiExport DlgMaterialPropertiesImp: public QDialog
{
    Q_OBJECT

public:
    explicit DlgMaterialPropertiesImp(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgMaterialPropertiesImp() override;
    App::Material getCustomMaterial() const;
    void setCustomMaterial(const App::Material& mat);
    App::Material getDefaultMaterial() const;
    void setDefaultMaterial(const App::Material& mat);

private:
    void setupConnections();
    void onAmbientColorChanged();
    void onDiffuseColorChanged();
    void onEmissiveColorChanged();
    void onSpecularColorChanged();
    void onShininessValueChanged(int);
    void onTransparencyValueChanged(int);
    void onButtonReset();
    void onButtonDefault();
    void setButtonColors(const App::Material& mat);

private:
    std::unique_ptr<Ui_DlgMaterialProperties> ui;
    App::Material customMaterial;
    App::Material defaultMaterial;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGMATERIALPROPERTIES_IMP_H
