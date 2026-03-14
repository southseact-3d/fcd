#include "PartPyCXX.h"


namespace Part
{
PartExport Py::Object shape2pyshape(const TopoShape& shape)
{
    PyObject* ret = const_cast<TopoShape&>(shape).getPyObject();
    return Py::asObject(ret);
}

PartExport Py::Object shape2pyshape(const TopoDS_Shape& shape)
{
    return shape2pyshape(TopoShape(shape));
}

}  // namespace Part


namespace Py
{
template<>
bool TopoShape::accepts(PyObject* pyob) const
{
    return (pyob && PyObject_TypeCheck(pyob, &(Part::TopoShapePy::Type)));
}

// explicit template instantiation
template class PartExport ExtensionObject<Part::TopoShapePy>;
}  // namespace Py
