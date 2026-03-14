#include <Interface_Static.hxx>


#include "ImportExportSettings.h"
#include <App/Application.h>


namespace Part
{
namespace IGES
{

ImportExportSettings::ImportExportSettings()
{
    pGroup = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Part/IGES"
    );
}

bool ImportExportSettings::getSkipBlankEntities() const
{
    return pGroup->GetBool("SkipBlankEntities", true);
}

void ImportExportSettings::setSkipBlankEntities(bool on) const
{
    pGroup->SetBool("SkipBlankEntities", on);
}

bool ImportExportSettings::getBRepMode() const
{
    int value = Part::Interface::writeIgesBrepMode();
    return pGroup->GetBool("BrepMode", value > 0);
}

void ImportExportSettings::setBRepMode(bool on) const
{
    pGroup->SetBool("BrepMode", on);
    Part::Interface::writeIgesBrepMode(on ? 1 : 0);
}

Interface::Unit ImportExportSettings::getUnit() const
{
    return static_cast<Interface::Unit>(pGroup->GetInt("Unit", 0));
}

void ImportExportSettings::setUnit(Interface::Unit unit)
{
    pGroup->SetInt("Unit", static_cast<long>(unit));
    Part::Interface::writeIgesUnit(unit);
}

std::string ImportExportSettings::getCompany() const
{
    return pGroup->GetASCII("Company", Part::Interface::writeIgesHeaderCompany());
}

void ImportExportSettings::setCompany(const char* name)
{
    pGroup->SetASCII("Company", name);
    Part::Interface::writeIgesHeaderCompany(name);
}

std::string ImportExportSettings::getAuthor() const
{
    return pGroup->GetASCII("Author", Part::Interface::writeIgesHeaderAuthor());
}

void ImportExportSettings::setAuthor(const char* name)
{
    pGroup->SetASCII("Author", name);
    Part::Interface::writeIgesHeaderAuthor(name);
}

std::string ImportExportSettings::getProductName() const
{
    return Part::Interface::writeIgesHeaderProduct();
}

void ImportExportSettings::setProductName(const char* name)
{
    Part::Interface::writeIgesHeaderProduct(name);
}

}  // namespace IGES
}  // namespace Part
