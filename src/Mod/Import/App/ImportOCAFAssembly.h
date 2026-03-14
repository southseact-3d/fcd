#ifndef IMPORT_ImportOCAFAssembly_H
#define IMPORT_ImportOCAFAssembly_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <TopoDS_Shape.hxx>

#include <App/Material.h>
#include <Mod/Import/ImportGlobal.h>


class TDF_Label;
class TopLoc_Location;

namespace App
{
class Document;
class DocumentObject;
}  // namespace App
namespace Part
{
class Feature;
}

namespace Import
{

class ImportExport ImportOCAFAssembly
{
public:
    ImportOCAFAssembly(
        Handle(TDocStd_Document) h,
        App::Document* d,
        const std::string& name,
        App::DocumentObject* target
    );
    virtual ~ImportOCAFAssembly();
    void loadShapes();
    void loadAssembly();

protected:
    std::string getName(const TDF_Label& label);
    App::DocumentObject* targetObj;


private:
    void loadShapes(
        const TDF_Label& label,
        const TopLoc_Location&,
        const std::string& partname,
        const std::string& assembly,
        bool isRef,
        int dep
    );
    void createShape(const TDF_Label& label, const TopLoc_Location&, const std::string&);
    void createShape(const TopoDS_Shape& label, const TopLoc_Location&, const std::string&);
    virtual void applyColors(Part::Feature*, const std::vector<Base::Color>&)
    {}

private:
    Handle(TDocStd_Document) pDoc;
    App::Document* doc;
    Handle(XCAFDoc_ShapeTool) aShapeTool;
    Handle(XCAFDoc_ColorTool) aColorTool;
    std::string default_name;
    std::set<int> myRefShapes;
};


}  // namespace Import

#endif  // IMPORT_ImportOCAFAssembly_H
