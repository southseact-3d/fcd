#include <Interface_Static.hxx>


#include "ImportExportSettings.h"
#include <App/Application.h>


namespace Part
{
namespace STEP
{

ImportExportSettings::ImportExportSettings()
{
    pGroup = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Part/STEP"
    );
}

void ImportExportSettings::setVisibleExportDialog(bool on)
{
    pGroup->SetBool("VisibleExportDialog", on);
}

bool ImportExportSettings::isVisibleExportDialog() const
{
    return pGroup->GetBool("VisibleExportDialog", true);
}

void ImportExportSettings::setVisibleImportDialog(bool on)
{
    pGroup->SetBool("VisibleImportDialog", on);
}

bool ImportExportSettings::isVisibleImportDialog() const
{
    return pGroup->GetBool("VisibleImportDialog", true);
}

void ImportExportSettings::setWriteSurfaceCurveMode(bool on)
{
    ParameterGrp::handle grp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Part/General"
    );
    grp->SetInt("WriteSurfaceCurveMode", on ? 1 : 0);
    Interface_Static::SetIVal("write.surfacecurve.mode", on ? 1 : 0);
}

bool ImportExportSettings::getWriteSurfaceCurveMode() const
{
    ParameterGrp::handle grp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Part/General"
    );
    int writesurfacecurve = Interface_Static::IVal("write.surfacecurve.mode");
    writesurfacecurve = grp->GetInt("WriteSurfaceCurveMode", writesurfacecurve);
    return (writesurfacecurve == 0 ? false : true);
}

std::string ImportExportSettings::getScheme() const
{
    return pGroup->GetASCII("Scheme", Interface_Static::CVal("write.step.schema"));
}

void ImportExportSettings::setScheme(const char* scheme)
{
    pGroup->SetASCII("Scheme", scheme);
    Interface_Static::SetCVal("write.step.schema", scheme);
}

Interface::Unit ImportExportSettings::getUnit() const
{
    return static_cast<Interface::Unit>(pGroup->GetInt("Unit", 0));
}

void ImportExportSettings::setUnit(Interface::Unit unit)
{
    pGroup->SetInt("Unit", static_cast<long>(unit));
    Part::Interface::writeStepUnit(unit);
}

std::string ImportExportSettings::getCompany() const
{
    return pGroup->GetASCII("Company");
}

void ImportExportSettings::setCompany(const char* name)
{
    pGroup->SetASCII("Company", name);
}

std::string ImportExportSettings::getAuthor() const
{
    return pGroup->GetASCII("Author");
}

void ImportExportSettings::setAuthor(const char* name)
{
    pGroup->SetASCII("Author", name);
}

std::string ImportExportSettings::getProductName() const
{
    return Part::Interface::writeStepHeaderProduct();
}

void ImportExportSettings::setProductName(const char* name)
{
    Part::Interface::writeStepHeaderProduct(name);
}

}  // namespace STEP
}  // namespace Part
