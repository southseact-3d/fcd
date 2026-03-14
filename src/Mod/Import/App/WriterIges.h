#ifndef IMPORT_WRITER_IGES_H
#define IMPORT_WRITER_IGES_H

#include <Mod/Import/ImportGlobal.h>
#include <Base/FileInfo.h>
#include <TDocStd_Document.hxx>

namespace Import
{

class ImportExport WriterIges
{
public:
    explicit WriterIges(const Base::FileInfo& file);

    void write(Handle(TDocStd_Document) hDoc) const;

private:
    Base::FileInfo file;
};
}  // namespace Import

#endif  // IMPORT_WRITER_IGES_H
