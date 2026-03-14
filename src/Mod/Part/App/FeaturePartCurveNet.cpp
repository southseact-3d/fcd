#include <Base/Console.h>

#include "FeaturePartCurveNet.h"


using namespace Part;

PROPERTY_SOURCE(Part::CurveNet, Part::Feature)

CurveNet::CurveNet()
{
    ADD_PROPERTY(FileName, (""));
}

short CurveNet::mustExecute() const
{
    if (FileName.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* CurveNet::execute()
{
    Base::FileInfo fi(FileName.getValue());
    if (!fi.isReadable()) {
        Base::Console().log("CurveNet::execute() not able to open %s!\n", FileName.getValue());
        std::string error = std::string("Cannot open file ") + FileName.getValue();
        return new App::DocumentObjectExecReturn(error);
    }

    TopoShape aShape;
    aShape.read(FileName.getValue());
    this->Shape.setValue(aShape);

    return App::DocumentObject::StdReturn;
}
