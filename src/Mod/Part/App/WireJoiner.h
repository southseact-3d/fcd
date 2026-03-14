#ifndef PART_WIRE_JOINER_H
#define PART_WIRE_JOINER_H

#include <memory>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Version.hxx>
#include "TopoShape.h"

#include <Mod/Part/PartGlobal.h>

namespace Part
{

class PartExport WireJoiner: public BRepBuilderAPI_MakeShape
{
public:
    WireJoiner();
    ~WireJoiner() override;

    void addShape(const TopoShape& shape);
    void addShape(const std::vector<TopoShape>& shapes);
    void addShape(const std::vector<TopoDS_Shape>& shapes);

    void setOutline(bool enable = true);
    void setTightBound(bool enable = true);
    void setSplitEdges(bool enable = true);
    void setMergeEdges(bool enable = true);
    void setTolerance(double tolerance, double atol = 0.0);

    bool getOpenWires(TopoShape& shape, const char* op = "", bool noOriginal = true);
    bool getResultWires(TopoShape& shape, const char* op = "");

#if OCC_VERSION_HEX < 0x070600
    void Build() override;
#else
    void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) override;
#endif
    const TopTools_ListOfShape& Modified(const TopoDS_Shape& SThatModifies) override;
    const TopTools_ListOfShape& Generated(const TopoDS_Shape& SThatGenerates) override;
    Standard_Boolean IsDeleted(const TopoDS_Shape& SDeleted) override;

private:
    class WireJoinerP;
    std::unique_ptr<WireJoinerP> pimpl;
};

}  // namespace Part

#endif  // PART_WIRE_JOINER_H
