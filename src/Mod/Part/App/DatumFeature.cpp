#include "DatumFeature.h"
#include "OCCError.h"
#include "PartPyCXX.h"


using namespace Part;
using namespace Attacher;

PROPERTY_SOURCE_ABSTRACT_WITH_EXTENSIONS(Part::Datum, Part::Feature)

Datum::Datum()
{
    AttachExtension::initExtension(this);
    touch();
}

Datum::~Datum() = default;

void Datum::onDocumentRestored()
{
    // This seems to be the only way to make the ViewProvider display the datum feature
    AttachmentSupport.touch();
    Part::Feature::onDocumentRestored();
}

TopoDS_Shape Datum::getShape() const
{
    Part::TopoShape sh = Shape.getShape();
    sh.setPlacement(Placement.getValue());
    return sh.getShape();
}

App::DocumentObject* Datum::getSubObject(
    const char* subname,
    PyObject** pyObj,
    Base::Matrix4D* pmat,
    bool transform,
    int depth
) const
{
    // For the sake of simplicity, we don't bother to check for subname, just
    // return the shape as it is, because a datum object only holds shape with
    // one single geometry element.
    (void)subname;
    (void)depth;

    if (pmat && transform) {
        *pmat *= Placement.getValue().toMatrix();
    }

    if (!pyObj) {
        return const_cast<Datum*>(this);
    }

    Base::PyGILStateLocker lock;
    PY_TRY
    {
        TopoShape ts(getShape().Located(TopLoc_Location()));
        if (pmat && !ts.isNull()) {
            ts.transformShape(*pmat, false, true);
        }
        *pyObj = Py::new_reference_to(shape2pyshape(ts.getShape()));
        return const_cast<Datum*>(this);
    }
    PY_CATCH_OCC
}

Base::Vector3d Datum::getBasePoint() const
{
    return Placement.getValue().getPosition();
}
