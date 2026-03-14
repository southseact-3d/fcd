#ifndef PART_IGES_IMPORTEXPORTSETTINGS_H
#define PART_IGES_IMPORTEXPORTSETTINGS_H

#include <Mod/Part/App/Interface.h>
#include <Base/Parameter.h>

#include <Mod/Part/PartGlobal.h>

namespace Part
{

namespace IGES
{

class PartExport ImportExportSettings
{
public:
    ImportExportSettings();

    bool getSkipBlankEntities() const;
    void setSkipBlankEntities(bool) const;

    bool getBRepMode() const;
    void setBRepMode(bool) const;

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

}  // namespace IGES
}  // namespace Part

#endif
