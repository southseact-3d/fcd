#ifndef MATERIAL_MODELMANAGERLOCAL_H
#define MATERIAL_MODELMANAGERLOCAL_H

#include <memory>

#include <Mod/Material/MaterialGlobal.h>

#include <QMutex>

#include "Exceptions.h"
#include "FolderTree.h"
#include "Model.h"
#include "ModelLibrary.h"

namespace Materials
{

class MaterialsExport ModelManagerLocal: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    ModelManagerLocal();
    ~ModelManagerLocal() override = default;

    static void cleanup();
    void refresh();

    std::shared_ptr<std::list<std::shared_ptr<ModelLibrary>>> getLibraries();
    void createLibrary(const QString& libraryName,
                       const QString& directory,
                       const QString& icon,
                       bool readOnly = true);
    void renameLibrary(const QString& libraryName, const QString& newName);
    void changeIcon(const QString& libraryName, const QString& icon);
    void removeLibrary(const QString& libraryName);
    std::shared_ptr<std::vector<LibraryObject>>
    libraryModels(const QString& libraryName);

    std::shared_ptr<std::map<QString, std::shared_ptr<Model>>> getModels()
    {
        return _modelMap;
    }
    std::shared_ptr<std::map<QString, std::shared_ptr<ModelTreeNode>>>
    getModelTree(std::shared_ptr<ModelLibrary> library, ModelFilter filter = ModelFilter_None) const
    {
        return library->getModelTree(filter);
    }
    std::shared_ptr<Model> getModel(const QString& uuid) const;
    std::shared_ptr<Model> getModelByPath(const QString& path) const;
    std::shared_ptr<Model> getModelByPath(const QString& path, const QString& lib) const;
    std::shared_ptr<ModelLibrary> getLibrary(const QString& name) const;

    static bool isModel(const QString& file);

private:
    static void initLibraries();

    static std::shared_ptr<std::list<std::shared_ptr<ModelLibraryLocal>>> _libraryList;
    static std::shared_ptr<std::map<QString, std::shared_ptr<Model>>> _modelMap;
    static QMutex _mutex;
};

}  // namespace Materials

#endif  // MATERIAL_MODELMANAGERLOCAL_H