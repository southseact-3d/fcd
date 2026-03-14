#include <Standard_Version.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>


#include "ReaderStep.h"
#include <Base/Exception.h>
#include <Mod/Part/App/encodeFilename.h>

using namespace Import;

ReaderStep::ReaderStep(const Base::FileInfo& file)  // NOLINT
    : file {file}
{
#if OCC_VERSION_HEX >= 0x070800
    codePage = Resource_FormatType_UTF8;
#endif
}

void ReaderStep::read(Handle(TDocStd_Document) hDoc)  // NOLINT
{
    std::string utf8Name = file.filePath();
    std::string name8bit = Part::encodeFilename(utf8Name);
    STEPCAFControl_Reader aReader;
    aReader.SetColorMode(true);
    aReader.SetNameMode(true);
    aReader.SetLayerMode(true);
    aReader.SetSHUOMode(true);
#if OCC_VERSION_HEX < 0x070800
    if (aReader.ReadFile(name8bit.c_str()) != IFSelect_RetDone) {
#else
    Handle(StepData_StepModel) aStepModel = new StepData_StepModel;
    aStepModel->InternalParameters.InitFromStatic();
    aStepModel->SetSourceCodePage(codePage);
    if (aReader.ReadFile(name8bit.c_str(), aStepModel->InternalParameters) != IFSelect_RetDone) {
#endif
        throw Base::FileException("Cannot read STEP file", file);
    }

    aReader.Transfer(hDoc);
}
