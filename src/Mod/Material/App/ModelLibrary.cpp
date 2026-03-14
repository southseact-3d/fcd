#include <string>
#include <QLatin1Char>

#include <QFileInfo>

#include <App/Application.h>

#include "Exceptions.h"
#include "Model.h"
#include "ModelLibrary.h"
#include "ModelManager.h"


using namespace Materials;

TYPESYSTEM_SOURCE(Materials::ModelLibrary, Materials::Library)

ModelLibrary::ModelLibrary(const Library& other)
    : Library(other)
{}

ModelLibrary::ModelLibrary(const QString& libraryName,
                           const QString& dir,
                           const QString& iconPath,
                           bool readOnly)
    : Library(libraryName, dir, iconPath, readOnly)
{}

ModelLibrary::ModelLibrary()
{
}

std::shared_ptr<std::map<QString, std::shared_ptr<ModelTreeNode>>>
ModelLibrary::getModelTree(ModelFilter filter) const
{
    std::shared_ptr<std::map<QString, std::shared_ptr<ModelTreeNode>>> modelTree =
        std::make_shared<std::map<QString, std::shared_ptr<ModelTreeNode>>>();

    auto models = ModelManager::getManager().libraryModels(getName());
    for (auto& it : *models) {
        auto uuid = it.getUUID();
        auto path = it.getPath();
        auto filename = it.getName();

        auto model = ModelManager::getManager().getModel(getName(), uuid);
        if (ModelManager::passFilter(filter, model->getType())) {
            QStringList list = path.split(QLatin1Char('/'));

            // Start at the root
            std::shared_ptr<std::map<QString, std::shared_ptr<ModelTreeNode>>> node = modelTree;
            for (auto& itp : list) {
                // Add the folder only if it's not already there
                if (!node->contains(itp)) {
                    auto mapPtr =
                        std::make_shared<std::map<QString, std::shared_ptr<ModelTreeNode>>>();
                    std::shared_ptr<ModelTreeNode> child = std::make_shared<ModelTreeNode>();
                    child->setFolder(mapPtr);
                    (*node)[itp] = child;
                    node = mapPtr;
                }
                else {
                    node = (*node)[itp]->getFolder();
                }
            }
            std::shared_ptr<ModelTreeNode> child = std::make_shared<ModelTreeNode>();
            child->setUUID(uuid);
            child->setData(model);
            (*node)[filename] = child;
        }
    }

    return modelTree;
}

TYPESYSTEM_SOURCE(Materials::ModelLibraryLocal, Materials::ModelLibrary)

ModelLibraryLocal::ModelLibraryLocal(const Library& other)
    : ModelLibrary(other)
{
    setLocal(true);

    _modelPathMap = std::make_unique<std::map<QString, std::shared_ptr<Model>>>();
}

ModelLibraryLocal::ModelLibraryLocal(const QString& libraryName,
                                     const QString& dir,
                                     const QString& iconPath,
                                     bool readOnly)
    : ModelLibrary(libraryName, dir, iconPath, readOnly)
{
    setLocal(true);

    _modelPathMap = std::make_unique<std::map<QString, std::shared_ptr<Model>>>();
}

ModelLibraryLocal::ModelLibraryLocal()
{
    setLocal(true);

    _modelPathMap = std::make_unique<std::map<QString, std::shared_ptr<Model>>>();
}

std::shared_ptr<Model> ModelLibraryLocal::getModelByPath(const QString& path) const
{
    QString filePath = getRelativePath(path);
    try {
        std::shared_ptr<Model> model = _modelPathMap->at(filePath);
        return model;
    }
    catch (std::out_of_range&) {
        throw ModelNotFound();
    }
}

std::shared_ptr<Model> ModelLibraryLocal::addModel(const Model& model, const QString& path)
{
    QString filePath = getRelativePath(path);
    QFileInfo info(filePath);
    std::shared_ptr<Model> newModel = std::make_shared<Model>(model);
    newModel->setLibrary(getptr());
    newModel->setDirectory(getLibraryPath(filePath, info.fileName()));
    newModel->setFilename(info.fileName());

    (*_modelPathMap)[filePath] = newModel;

    return newModel;
}
