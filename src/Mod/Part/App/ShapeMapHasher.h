#ifndef PART_SHAPEMAPHASHER_H
#define PART_SHAPEMAPHASHER_H

#include <limits>

#include <Standard_Version.hxx>
#include <TopoDS_Shape.hxx>
#if OCC_VERSION_HEX >= 0x070800
# include <functional>
#endif

namespace Part
{

class ShapeMapHasher
{
public:
    size_t operator()(const TopoDS_Shape& theShape) const
    {
#if OCC_VERSION_HEX < 0x070800
        return theShape.HashCode(std::numeric_limits<int>::max());
#else
        return std::hash<TopoDS_Shape> {}(theShape);
#endif
    }
};

}  // namespace Part

#endif  // PART_SHAPEMAPHASHER_H
