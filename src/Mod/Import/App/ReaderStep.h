#ifndef IMPORT_READER_STEP_H
#define IMPORT_READER_STEP_H

#include <Mod/Import/ImportGlobal.h>
#include <Base/FileInfo.h>
#include <Resource_FormatType.hxx>
#include <TDocStd_Document.hxx>
#include <StepData_StepModel.hxx>
#include <Standard_Version.hxx>

namespace Import
{

class ImportExport ReaderStep
{
public:
    explicit ReaderStep(const Base::FileInfo& file);
    void setCodePage(Resource_FormatType cp)
    {
        codePage = cp;
    }
    void read(Handle(TDocStd_Document) hDoc);

private:
    Base::FileInfo file;
    Resource_FormatType codePage {};
};

}  // namespace Import

#endif  // IMPORT_READER_STEP_H
