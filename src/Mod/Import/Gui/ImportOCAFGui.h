#ifndef IMPORT_IMPORTOCAFGUI_H
#define IMPORT_IMPORTOCAFGUI_H

#include <Mod/Import/App/ImportOCAF2.h>

namespace ImportGui
{

class ImportOCAFGui: public Import::ImportOCAF2
{
public:
    ImportOCAFGui(Handle(TDocStd_Document) hDoc, App::Document* pDoc, const std::string& name);

private:
    void applyFaceColors(Part::Feature* part, const std::vector<Base::Color>& colors) override;
    void applyEdgeColors(Part::Feature* part, const std::vector<Base::Color>& colors) override;
    void applyLinkColor(App::DocumentObject* obj, int index, Base::Color color) override;
    void applyElementColors(
        App::DocumentObject* obj,
        const std::map<std::string, Base::Color>& colors
    ) override;
};

}  // namespace ImportGui

#endif  // IMPORT_IMPORTOCAFGUI_H
