#ifndef IMPEXPDXFGUI_H
#define IMPEXPDXFGUI_H

#include <gp_Pnt.hxx>

#include <App/Document.h>
#include <Gui/Document.h>
#include <Mod/Part/App/TopoShape.h>

#include <Mod/Import/App/dxf/ImpExpDxf.h>


namespace ImportGui
{
class ImpExpDxfReadGui: public Import::ImpExpDxfRead
{
public:
    ImpExpDxfReadGui(const std::string& filepath, App::Document* pcDoc);

protected:
    void ApplyGuiStyles(Part::Feature* object) const override;
    void ApplyGuiStyles(App::Link* object) const override;
    void ApplyGuiStyles(App::FeaturePython* object) const override;

private:
    Gui::Document* GuiDocument;
    int GetDrawStyle() const;
};
}  // namespace ImportGui

#endif  // IMPEXPDXFGUI_H
