#ifndef IMPORT_TOOLS_H
#define IMPORT_TOOLS_H

#include <limits>

#include <Quantity_ColorRGBA.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <Base/Color.h>

#include <Standard_Version.hxx>

namespace Import
{

struct ShapeHasher
{
    std::size_t operator()(const TopoDS_Shape& shape) const
    {
#if OCC_VERSION_HEX >= 0x070800
        return std::hash<TopoDS_Shape> {}(shape);
#else
        return shape.HashCode(std::numeric_limits<int>::max());
#endif
    }
};

struct LabelHasher
{
    std::size_t operator()(const TDF_Label& label) const
    {
#if OCC_VERSION_HEX >= 0x070800
        return std::hash<TDF_Label> {}(label);
#else
        return TDF_LabelMapHasher::HashCode(label, std::numeric_limits<int>::max());
#endif
    }
};

struct Tools
{
    static Base::Color convertColor(const Quantity_ColorRGBA& rgba);
    static Quantity_ColorRGBA convertColor(const Base::Color& col);
    static std::string labelName(TDF_Label label);
    static void printLabel(
        TDF_Label label,
        Handle(XCAFDoc_ShapeTool) aShapeTool,
        Handle(XCAFDoc_ColorTool) aColorTool,
        const char* msg = nullptr
    );

    static void dumpLabels(
        TDF_Label label,
        Handle(XCAFDoc_ShapeTool) aShapeTool,
        Handle(XCAFDoc_ColorTool) aColorTool,
        int depth = 0
    );
};

}  // namespace Import

#endif  // IMPORT_IMPORTOCAF2_H
