#include <Base/Console.h>

#include "SketchObjectSF.h"


using namespace Sketcher;

PROPERTY_SOURCE(Sketcher::SketchObjectSF, Part::Part2DObject)

SketchObjectSF::SketchObjectSF()
{
    ADD_PROPERTY_TYPE(
        SketchFlatFile,
        (nullptr),
        "",
        (App::PropertyType)(App::Prop_None),
        "SketchFlat file (*.skf) which defines this sketch"
    );
}

short SketchObjectSF::mustExecute() const
{
    if (SketchFlatFile.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* SketchObjectSF::execute()
{
    Base::Console().warning(
        "%s: This feature is deprecated and won't be longer supported in future FreeCAD versions\n",
        this->getNameInDocument()
    );
    // do nothing
    return App::DocumentObject::StdReturn;
}
