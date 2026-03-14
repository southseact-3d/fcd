#ifndef IMPORT_EXPORTOCAFGUI_H
#define IMPORT_EXPORTOCAFGUI_H

#include <Mod/Import/App/ExportOCAF.h>

namespace ImportGui
{

class ExportOCAFGui: public Import::ExportOCAF
{
public:
    ExportOCAFGui(Handle(TDocStd_Document) hDoc, bool explicitPlacement);
    void findColors(Part::Feature* part, std::vector<Base::Color>& colors) const override;
};

}  // namespace ImportGui

#endif  // IMPORT_EXPORTOCAFGUI_H
