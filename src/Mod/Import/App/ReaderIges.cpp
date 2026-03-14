#include <IGESControl_Controller.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESToBRep_Actor.hxx>
#include <Standard_Version.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>


#include "ReaderIges.h"
#include <Base/Exception.h>
#include <App/Application.h>
#include <Mod/Part/App/encodeFilename.h>

using namespace Import;

ReaderIges::ReaderIges(const Base::FileInfo& file)  // NOLINT
    : file {file}
{}

void ReaderIges::read(Handle(TDocStd_Document) hDoc)  // NOLINT
{
    Base::Reference<ParameterGrp> hGrp = App::GetApplication()
                                             .GetUserParameter()
                                             .GetGroup("BaseApp")
                                             ->GetGroup("Preferences")
                                             ->GetGroup("Mod/Part")
                                             ->GetGroup("IGES");
    std::string utf8Name = file.filePath();
    std::string name8bit = Part::encodeFilename(utf8Name);

    IGESControl_Controller::Init();
    IGESCAFControl_Reader aReader;
    // http://www.opencascade.org/org/forum/thread_20603/?forum=3
    aReader.SetReadVisible(hGrp->GetBool("SkipBlankEntities", true));
    aReader.SetColorMode(true);
    aReader.SetNameMode(true);
    aReader.SetLayerMode(true);
    if (aReader.ReadFile(name8bit.c_str()) != IFSelect_RetDone) {
        throw Base::FileException("Cannot read IGES file", file);
    }

    aReader.Transfer(hDoc);

    // http://opencascade.blogspot.de/2009/03/unnoticeable-memory-leaks-part-2.html
    Handle(IGESToBRep_Actor)::DownCast(aReader.WS()->TransferReader()->Actor())
        ->SetModel(new IGESData_IGESModel);
}
