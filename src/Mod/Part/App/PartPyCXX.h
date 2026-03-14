#ifndef PART_PYCXX_H
#define PART_PYCXX_H

#include <CXX/Extensions.hxx>

#include <Mod/Part/PartGlobal.h>
#include <Mod/Part/App/TopoShapePy.h>


namespace Py
{
using TopoShape = ExtensionObject<Part::TopoShapePy>;
template<>
bool TopoShape::accepts(PyObject* pyob) const;
}  // namespace Py

namespace Part
{
PartExport Py::Object shape2pyshape(const TopoShape& shape);
PartExport Py::Object shape2pyshape(const TopoDS_Shape& shape);
PartExport void getPyShapes(PyObject* obj, std::vector<TopoShape>& shapes);
PartExport std::vector<TopoShape> getPyShapes(PyObject* obj);
}  // namespace Part

#endif  // PART_PYCXX_H
