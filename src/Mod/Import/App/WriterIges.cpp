#include <IGESControl_Controller.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESToBRep_Actor.hxx>


#include "WriterIges.h"
#include <Base/Exception.h>
#include <App/Application.h>
#include <Mod/Part/App/encodeFilename.h>
#include <Mod/Part/App/Interface.h>

using namespace Import;

WriterIges::WriterIges(const Base::FileInfo& file)  // NOLINT
    : file {file}
{}

void WriterIges::write(Handle(TDocStd_Document) hDoc) const  // NOLINT
{
    std::string utf8Name = file.filePath();
    std::string name8bit = Part::encodeFilename(utf8Name);

    IGESControl_Controller::Init();
    IGESCAFControl_Writer writer;
    IGESData_GlobalSection header = writer.Model()->GlobalSection();
    header.SetAuthorName(new TCollection_HAsciiString(Part::Interface::writeIgesHeaderAuthor()));
    header.SetCompanyName(new TCollection_HAsciiString(Part::Interface::writeIgesHeaderCompany()));
    header.SetSendName(new TCollection_HAsciiString(Part::Interface::writeIgesHeaderProduct()));
    writer.Model()->SetGlobalSection(header);
    writer.Transfer(hDoc);
    Standard_Boolean ret = writer.Write(name8bit.c_str());
    if (!ret) {
        throw Base::FileException("Cannot open file: ", file);
    }
}
