#ifndef PART_STEP_IMPORTEXPORTSETTINGS_H
#define PART_STEP_IMPORTEXPORTSETTINGS_H

#include <Mod/Part/App/Interface.h>
#include <Base/Parameter.h>

#include <Mod/Part/PartGlobal.h>

namespace Part
{
namespace STEP
{

class PartExport ImportExportSettings
{
public:
    ImportExportSettings();

    void setVisibleExportDialog(bool);
    bool isVisibleExportDialog() const;

    void setVisibleImportDialog(bool);
    bool isVisibleImportDialog() const;

    void setWriteSurfaceCurveMode(bool);
    bool getWriteSurfaceCurveMode() const;

    std::string getScheme() const;
    void setScheme(const char*);

    Interface::Unit getUnit() const;
    void setUnit(Interface::Unit);

    std::string getCompany() const;
    void setCompany(const char*);

    std::string getAuthor() const;
    void setAuthor(const char*);

    std::string getProductName() const;
    void setProductName(const char*);

private:
    ParameterGrp::handle pGroup;
};

}  // namespace STEP
}  // namespace Part

#endif
