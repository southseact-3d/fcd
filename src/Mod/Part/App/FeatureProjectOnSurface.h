#ifndef PART_FEATUREPROJECTONSURFACE_H
#define PART_FEATUREPROJECTONSURFACE_H

#include "PartFeature.h"
#include <App/PropertyLinks.h>
#include <App/PropertyUnits.h>

#include <Mod/Part/PartGlobal.h>

namespace Part
{

class PartExport ProjectOnSurface: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::ProjectOnSurface);

public:
    ProjectOnSurface();

    App::PropertyEnumeration Mode;
    App::PropertyLength Height;
    App::PropertyDistance Offset;
    App::PropertyDirection Direction;
    App::PropertyLinkSub SupportFace;
    App::PropertyLinkSubList Projection;

    static constexpr const char* AllMode = "All";
    static constexpr const char* FacesMode = "Faces";
    static constexpr const char* EdgesMode = "Edges";

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    const char* getViewProviderName() const override;
    //@}

private:
    void tryExecute();
    TopoDS_Face getSupportFace() const;
    std::vector<TopoDS_Shape> getProjectionShapes() const;
    std::vector<TopoDS_Shape> createProjectedWire(
        const TopoDS_Shape& shape,
        const TopoDS_Face& supportFace,
        const gp_Dir& dir
    );
    TopoDS_Face createFaceFromWire(
        const std::vector<TopoDS_Shape>& wires,
        const TopoDS_Face& supportFace
    ) const;
    TopoDS_Face createFaceFromParametricWire(
        const std::vector<TopoDS_Wire>& wires,
        const TopoDS_Face& supportFace
    ) const;
    TopoDS_Shape createSolidIfHeight(const TopoDS_Face& face) const;
    std::vector<TopoDS_Wire> createWiresFromWires(
        const std::vector<TopoDS_Shape>& wires,
        const TopoDS_Face& supportFace
    ) const;
    std::vector<TopoDS_Wire> getWires(const TopoDS_Face& face) const;
    std::vector<TopoDS_Shape> projectFace(
        const TopoDS_Face& face,
        const TopoDS_Face& supportFace,
        const gp_Dir& dir
    );
    std::vector<TopoDS_Shape> projectWire(
        const TopoDS_Shape& wire,
        const TopoDS_Face& supportFace,
        const gp_Dir& dir
    );
    TopoDS_Wire fixWire(const TopoDS_Shape& shape, const TopoDS_Face& supportFace) const;
    TopoDS_Wire fixWire(const std::vector<TopoDS_Edge>& edges, const TopoDS_Face& supportFace) const;
    std::vector<TopoDS_Shape> filterShapes(const std::vector<TopoDS_Shape>& shapes) const;
    TopoDS_Shape createCompound(const std::vector<TopoDS_Shape>& shapes);
    TopLoc_Location getOffsetPlacement() const;
};

}  // namespace Part


#endif  // PART_FEATUREPROJECTONSURFACE_H
