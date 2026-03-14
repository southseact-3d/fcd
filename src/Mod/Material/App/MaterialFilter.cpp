#include <App/Application.h>

#include "Exceptions.h"
#include "MaterialFilter.h"
#include "MaterialManager.h"
#include "Materials.h"


using namespace Materials;

TYPESYSTEM_SOURCE(Materials::MaterialFilterOptions, Base::BaseClass)

MaterialFilterOptions::MaterialFilterOptions()
{
    auto param = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Material/Editor");
    _includeFavorites = param->GetBool("ShowFavorites", true);
    _includeRecent = param->GetBool("ShowRecent", true);
    _includeFolders = param->GetBool("ShowEmptyFolders", false);
    _includeLibraries = param->GetBool("ShowEmptyLibraries", true);
    _includeLegacy = param->GetBool("ShowLegacy", false);
}

MaterialFilterTreeWidgetOptions::MaterialFilterTreeWidgetOptions()
{
    auto param = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Material/TreeWidget");
    _includeFavorites = param->GetBool("ShowFavorites", true);
    _includeRecent = param->GetBool("ShowRecent", true);
    _includeFolders = param->GetBool("ShowEmptyFolders", false);
    _includeLibraries = param->GetBool("ShowEmptyLibraries", true);
    _includeLegacy = param->GetBool("ShowLegacy", false);
}

//===

TYPESYSTEM_SOURCE(Materials::MaterialFilter, Base::BaseClass)

MaterialFilter::MaterialFilter()
    : _required()
    , _requiredComplete()
    , _requirePhysical(false)
    , _requireAppearance(false)
{}

bool MaterialFilter::modelIncluded(const Material& material) const
{
    if (_requirePhysical) {
        if (!material.hasPhysicalProperties()) {
            return false;
        }
    }
    if (_requireAppearance) {
        if (!material.hasAppearanceProperties()) {
            return false;
        }
    }
    for (const auto& complete : _requiredComplete) {
        if (!material.isModelComplete(complete)) {
            return false;
        }
    }
    for (const auto& required : _required) {
        if (!material.hasModel(required)) {
            return false;
        }
    }

    return true;
}

bool MaterialFilter::modelIncluded(const QString& uuid) const
{
    try {
        auto material = MaterialManager::getManager().getMaterial(uuid);
        return modelIncluded(*material);
    }
    catch (const MaterialNotFound&) {
    }
    return false;
}

void MaterialFilter::addRequired(const QString& uuid)
{
    // Ignore any uuids already present
    if (!_requiredComplete.contains(uuid)) {
        _required.insert(uuid);
    }
}

void MaterialFilter::addRequiredComplete(const QString& uuid)
{
    if (_required.contains(uuid)) {
        // Completeness takes priority
        _required.remove(uuid);
    }
    _requiredComplete.insert(uuid);
}

void MaterialFilter::clear()
{
    _required.clear();
    _requiredComplete.clear();
}
