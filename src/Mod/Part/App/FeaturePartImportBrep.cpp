#include <fcntl.h>


#include <Base/Console.h>
#include <Base/FileInfo.h>

#include "FeaturePartImportBrep.h"


using namespace Part;

PROPERTY_SOURCE(Part::ImportBrep, Part::Feature)


ImportBrep::ImportBrep()
{
    ADD_PROPERTY(FileName, (""));
}

short ImportBrep::mustExecute() const
{
    if (FileName.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* ImportBrep::execute()
{
    Base::FileInfo fi(FileName.getValue());
    if (!fi.isReadable()) {
        Base::Console().log("ImportBrep::execute() not able to open %s!\n", FileName.getValue());
        std::string error = std::string("Cannot open file ") + FileName.getValue();
        return new App::DocumentObjectExecReturn(error);
    }

    TopoShape aShape;
    aShape.importBrep(FileName.getValue());
    this->Shape.setValue(aShape);

    return App::DocumentObject::StdReturn;
}
