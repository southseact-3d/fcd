#include <fcntl.h>


#include <Base/Console.h>
#include <Base/FileInfo.h>

#include "FeaturePartImportStep.h"


using namespace Part;

PROPERTY_SOURCE(Part::ImportStep, Part::Feature)


ImportStep::ImportStep()
{
    ADD_PROPERTY(FileName, (""));
}

short ImportStep::mustExecute() const
{
    if (FileName.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* ImportStep::execute()
{
    Base::FileInfo fi(FileName.getValue());
    if (!fi.isReadable()) {
        Base::Console().log("ImportStep::execute() not able to open %s!\n", FileName.getValue());
        std::string error = std::string("Cannot open file ") + FileName.getValue();
        return new App::DocumentObjectExecReturn(error);
    }

    TopoShape aShape;
    aShape.importStep(FileName.getValue());
    this->Shape.setValue(aShape);

    return App::DocumentObject::StdReturn;
}
