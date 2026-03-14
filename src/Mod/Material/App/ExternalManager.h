#ifndef MATERIAL_EXTERNALMANAGER_H
#define MATERIAL_EXTERNALMANAGER_H

#include <Base/Parameter.h>
#include <CXX/Objects.hxx>

#include <Mod/Material/MaterialGlobal.h>

class QMutex;
class QString;
class QByteArray;

namespace Materials
{

class Library;
class LibraryObject;
class Material;
class Model;
class MaterialFilter;
class MaterialFilterOptions;

class MaterialsExport ExternalManager: public ParameterGrp::ObserverType
{
public:

    static ExternalManager* getManager();

    /// Observer message from the ParameterGrp
    void OnChange(ParameterGrp::SubjectType& rCaller, ParameterGrp::MessageType Reason) override;

    // Library management
    std::shared_ptr<std::vector<std::shared_ptr<Library>>> libraries();
    std::shared_ptr<std::vector<std::shared_ptr<Library>>> modelLibraries();
    std::shared_ptr<std::vector<std::shared_ptr<Library>>> materialLibraries();
    std::shared_ptr<Library> getLibrary(const QString& name);
    void createLibrary(const QString& libraryName,
                       const QByteArray& icon,
                       bool readOnly = true);
    void renameLibrary(const QString& libraryName, const QString& newName);
    void changeIcon(const QString& libraryName, const QByteArray& icon);
    void removeLibrary(const QString& libraryName);
    std::shared_ptr<std::vector<LibraryObject>> libraryModels(const QString& libraryName);
    std::shared_ptr<std::vector<LibraryObject>> libraryMaterials(const QString& libraryName);
    std::shared_ptr<std::vector<LibraryObject>>
    libraryMaterials(const QString& libraryName,
                     const MaterialFilter& filter,
                     const MaterialFilterOptions& options);
    std::shared_ptr<std::vector<QString>> libraryFolders(const QString& libraryName);

    // Folder management
    void createFolder(const QString& libraryName, const QString& path);
    void renameFolder(const QString& libraryName,
                      const QString& oldPath,
                      const QString& newPath);
    void deleteRecursive(const QString& libraryName, const QString& path);

    // Model management
    std::shared_ptr<Model> getModel(const QString& uuid);
    void
    addModel(const QString& libraryName, const QString& path, const Model& model);
    void
    migrateModel(const QString& libraryName, const QString& path, const Model& model);
    void updateModel(const QString& libraryName,
                     const QString& path,
                     const Model& model);
    void setModelPath(const QString& libraryName, const QString& path, const QString& uuid);
    void renameModel(const QString& libraryName, const QString& name, const QString& uuid);
    void moveModel(const QString& libraryName, const QString& path, const QString& uuid);
    void removeModel(const QString& uuid);

    // Material management
    std::shared_ptr<Material> getMaterial(const QString& uuid);
    void addMaterial(const QString& libraryName,
                     const QString& path,
                     const Material& material);
    void migrateMaterial(const QString& libraryName,
                     const QString& path,
                     const Material& material);
    void updateMaterial(const QString& libraryName,
                        const QString& path,
                        const Material& material);
    void setMaterialPath(const QString& libraryName, const QString& path, const QString& uuid);
    void renameMaterial(const QString& libraryName, const QString& name, const QString& uuid);
    void moveMaterial(const QString& libraryName, const QString& path, const QString& uuid);
    void removeMaterial(const QString& uuid);

private:
    ExternalManager();
    ~ExternalManager() override;

    static void initManager();
    void getConfiguration();
    void instantiate();
    void connect();
    bool checkMaterialLibraryType(const Py::Object& entry);
    std::shared_ptr<Library> libraryFromObject(const Py::Object& entry);
    bool checkMaterialLibraryObjectType(const Py::Object& entry);
    LibraryObject materialLibraryObjectTypeFromObject(const Py::Object& entry);
    bool checkModelObjectType(const Py::Object& entry);
    std::shared_ptr<Model> modelFromObject(const Py::Object& entry, const QString& uuid);
    bool checkMaterialObjectType(const Py::Object& entry);
    std::shared_ptr<Material> materialFromObject(const Py::Object& entry, const QString& uuid);

    static ExternalManager* _manager;
    static QMutex _mutex;

    // COnfiguration
    ParameterGrp::handle _hGrp;
    std::string _moduleName;
    std::string _className;
    bool _instantiated;

    Py::Object _managerObject;
};

}  // namespace Materials

#endif  // MATERIAL_EXTERNALMANAGER_H