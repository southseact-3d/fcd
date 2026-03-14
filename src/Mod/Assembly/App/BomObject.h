#ifndef ASSEMBLY_BomObject_H
#define ASSEMBLY_BomObject_H

#include <App/PropertyFile.h>

#include <Mod/Assembly/AssemblyGlobal.h>

#include <Mod/Spreadsheet/App/Sheet.h>
#include <App/PropertyLinks.h>

namespace App
{
class DocumentObject;
}

namespace Assembly
{

class AssemblyObject;

class BomDataElement
{
public:
    BomDataElement(std::string objName, std::string columnName, std::string value)
        : objName(objName)
        , columnName(columnName)
        , value(value)
    {}
    ~BomDataElement()
    {}

    std::string objName;
    std::string columnName;
    std::string value;
};

class AssemblyExport BomObject: public Spreadsheet::Sheet
{
    PROPERTY_HEADER_WITH_OVERRIDE(Assembly::BomObject);

public:
    BomObject();
    ~BomObject() override;

    PyObject* getPyObject() override;

    const char* getViewProviderName() const override
    {
        return "AssemblyGui::ViewProviderBom";
    }

    App::DocumentObjectExecReturn* execute() override;

    void generateBOM();
    void addObjectToBom(App::DocumentObject* obj, size_t row, std::string index, bool isMirrored = false);
    void addObjectChildrenToBom(std::vector<App::DocumentObject*> objs, size_t& row, std::string index);
    void saveCustomColumnData();
    bool isObjMirrored(App::DocumentObject* obj);

    AssemblyObject* getAssembly() const;

    bool hasQuantityColumn() const;
    int getColumnIndex(std::string name) const;
    std::string getText(size_t row, size_t col) const;

    App::PropertyStringList columnsNames;
    App::PropertyBool detailSubAssemblies;
    App::PropertyBool detailParts;
    App::PropertyBool onlyParts;

    std::vector<BomDataElement> dataElements;
    std::vector<App::DocumentObject*> obj_list;
    std::vector<bool> obj_mirrored_list;

private:
    std::string getBomPropertyValue(App::DocumentObject* obj, const std::string& baseName);

    std::string mirroredSuffix;
};


}  // namespace Assembly


#endif  // ASSEMBLY_BomObject_H
