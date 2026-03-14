#ifndef IMPORT_READER_GLTF_H
#define IMPORT_READER_GLTF_H

#include <Mod/Import/ImportGlobal.h>
#include <Base/FileInfo.h>
#include <TDocStd_Document.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopoDS_Shape.hxx>

namespace Import
{

class ImportExport ReaderGltf
{
public:
    explicit ReaderGltf(const Base::FileInfo& file);

    void read(Handle(TDocStd_Document) hDoc);
    bool cleanup() const;
    void setCleanup(bool);

private:
    TopoDS_Shape fixShape(TopoDS_Shape);
    void processDocument(Handle(TDocStd_Document) hDoc);
    TopoDS_Shape processSubShapes(Handle(TDocStd_Document) hDoc, const TDF_LabelSequence& subShapeLabels);

private:
    Base::FileInfo file;
    bool clean = true;
};

}  // namespace Import

#endif  // IMPORT_READER_GLTF_H
