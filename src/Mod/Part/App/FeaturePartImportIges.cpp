#include <fcntl.h>


#include <Base/Console.h>
#include <Base/FileInfo.h>

#include "FeaturePartImportIges.h"


using namespace Part;

PROPERTY_SOURCE(Part::ImportIges, Part::Feature)


ImportIges::ImportIges()
{
    ADD_PROPERTY(FileName, (""));
}

short ImportIges::mustExecute() const
{
    if (FileName.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* ImportIges::execute()
{
    Base::FileInfo fi(FileName.getValue());
    if (!fi.isReadable()) {
        Base::Console().log("ImportIges::execute() not able to open %s!\n", FileName.getValue());
        std::string error = std::string("Cannot open file ") + FileName.getValue();
        return new App::DocumentObjectExecReturn(error);
    }

    TopoShape aShape;
    aShape.importIges(FileName.getValue());
    this->Shape.setValue(aShape);

    return App::DocumentObject::StdReturn;
}
