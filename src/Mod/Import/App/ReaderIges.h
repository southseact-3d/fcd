#ifndef IMPORT_READER_IGES_H
#define IMPORT_READER_IGES_H

#include <Mod/Import/ImportGlobal.h>
#include <Base/FileInfo.h>
#include <TDocStd_Document.hxx>

namespace Import
{

class ImportExport ReaderIges
{
public:
    explicit ReaderIges(const Base::FileInfo& file);

    void read(Handle(TDocStd_Document) hDoc);

private:
    Base::FileInfo file;
};

}  // namespace Import

#endif  // IMPORT_READER_IGES_H
