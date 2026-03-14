#include <sstream>


#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <STEPControl_Reader.hxx>
#include <Standard_Version.hxx>
#include <StepData_StepModel.hxx>

#include "StepShape.h"


using namespace Import;

StepShape::StepShape() = default;

StepShape::~StepShape() = default;

int StepShape::read(const char* fileName)
{
    STEPControl_Reader aReader;

    Base::FileInfo fi(fileName);

    if (!fi.exists()) {
        std::stringstream str;
        str << "File '" << fileName << "' does not exist!";
        throw Base::FileException(str.str().c_str());
    }

    if (aReader.ReadFile((Standard_CString)fileName) != IFSelect_RetDone) {
        throw Base::FileException("Cannot open STEP file");
    }


    Handle(TColStd_HSequenceOfTransient) list = aReader.GiveList();

    // Use method StepData_StepModel::NextNumberForLabel to find its rank with the following:
    // Standard_CString label = "#...";
    Handle(StepData_StepModel) model = aReader.StepModel();


    std::cout << "dump of step header:" << std::endl;
    model->DumpHeader(std::cout);

    for (int nent = 1; nent <= model->NbEntities(); nent++) {
        Handle(Standard_Transient) entity = model->Entity(nent);
        std::cout << "label entity " << nent << ":";
        model->PrintLabel(entity, std::cout);
        std::cout << ";" << entity->DynamicType()->Name() << std::endl;
    }

    return 0;
}
