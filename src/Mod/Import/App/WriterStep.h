#ifndef IMPORT_WRITER_STEP_H
#define IMPORT_WRITER_STEP_H

#include <Mod/Import/ImportGlobal.h>
#include <Base/FileInfo.h>
#include <TDocStd_Document.hxx>

namespace Import
{

class ImportExport WriterStep
{
public:
    explicit WriterStep(const Base::FileInfo& file);

    void write(Handle(TDocStd_Document) hDoc) const;

private:
    Base::FileInfo file;
};
}  // namespace Import

#endif  // IMPORT_WRITER_GLTF_H
