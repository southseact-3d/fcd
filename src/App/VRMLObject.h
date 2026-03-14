#ifndef APP_VRMLOROBJECT_H
#define APP_VRMLOROBJECT_H

#include "GeoFeature.h"
#include "PropertyFile.h"


namespace App
{

class AppExport VRMLObject: public GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::VRMLObject);

public:
    /// Constructor
    VRMLObject();

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderVRMLObject";
    }
    DocumentObjectExecReturn* execute() override
    {
        return DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;
    void SaveDocFile(Base::Writer& writer) const override;
    void RestoreDocFile(Base::Reader& reader) override;

    // NOLINTBEGIN
    PropertyFileIncluded VrmlFile;
    PropertyStringList Urls;
    PropertyStringList Resources;
    // NOLINTEND

protected:
    void onChanged(const App::Property*) override;

private:
    std::string getRelativePath(const std::string& prefix, const std::string& resource) const;
    static std::string fixRelativePath(const std::string& name, const std::string& resource);
    static void makeDirectories(const std::string& path, const std::string& subdir);
    bool restoreTextureFinished(Base::Reader& reader);
    void reloadFile();

private:
    mutable std::string vrmlPath;
    mutable int indexRestore {0};
    mutable int indexSave {0};
    mutable bool restoreData {false};
};

}  // namespace App


#endif  // APP_INVENTOROBJECT_H
