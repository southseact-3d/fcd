#ifndef IMPORT_EXPORTOCAF2_H
#define IMPORT_EXPORTOCAF2_H

#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <TDocStd_Document.hxx>

#include <Mod/Import/ImportGlobal.h>
#include "Tools.h"

namespace App
{
class DocumentObject;
}

namespace Part
{
class TopoShape;
}

namespace Import
{

struct ImportExport ExportOCAFOptions
{
    ExportOCAFOptions();
    Base::Color defaultColor;
    bool exportHidden = true;
    bool keepPlacement = false;
};

class ImportExport ExportOCAF2
{
public:
    using GetShapeColorsFunc
        = std::function<std::map<std::string, Base::Color>(App::DocumentObject*, const char*)>;
    explicit ExportOCAF2(Handle(TDocStd_Document) hDoc, GetShapeColorsFunc func = GetShapeColorsFunc());

    static ExportOCAFOptions customExportOptions();
    void setExportOptions(ExportOCAFOptions opts)
    {
        options = opts;
    }
    void setExportHiddenObject(bool enable)
    {
        options.exportHidden = enable;
    }
    void setKeepPlacement(bool enable)
    {
        options.keepPlacement = enable;
    }
    void exportObjects(std::vector<App::DocumentObject*>& objs, const char* name = nullptr);
    bool canFallback(std::vector<App::DocumentObject*> objs);

private:
    TDF_Label exportObject(
        App::DocumentObject* obj,
        const char* sub,
        TDF_Label parent,
        const char* name = nullptr
    );
    void setupObject(
        TDF_Label label,
        App::DocumentObject* obj,
        const Part::TopoShape& shape,
        const std::string& prefix,
        const char* name = nullptr,
        bool force = false
    );
    void setName(TDF_Label label, App::DocumentObject* obj, const char* name = nullptr);
    TDF_Label findComponent(const char* subname, TDF_Label label, TDF_LabelSequence& labels);

private:
    Handle(TDocStd_Document) pDoc;
    Handle(XCAFDoc_ShapeTool) aShapeTool;
    Handle(XCAFDoc_ColorTool) aColorTool;

    std::unordered_map<App::DocumentObject*, TDF_Label> myObjects;

    std::unordered_map<TDF_Label, std::vector<std::string>, LabelHasher> myNames;

    std::set<std::pair<App::DocumentObject*, std::string>> mySetups;

    std::vector<App::DocumentObject*> groupLinks;

    GetShapeColorsFunc getShapeColors;

    ExportOCAFOptions options;
};

}  // namespace Import

#endif  // IMPORT_EXPORTOCAF2_H
