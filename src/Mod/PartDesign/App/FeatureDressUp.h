#ifndef PARTDESIGN_DressUp_H
#define PARTDESIGN_DressUp_H

#include "FeatureAddSub.h"

namespace PartDesign
{

class PartDesignExport DressUp: public PartDesign::FeatureAddSub
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::DressUp);

public:
    DressUp();

    /**
     * Base feature and it's subelements to which dressup operation will be applied to.
     * Unlike Feature::BaseFeature it includes Sublinks and set not only inside a body.
     * But for consistency if BaseFeature is nonzero this links to the same body as it.
     */
    App::PropertyLinkSub Base;
    App::PropertyBool SupportTransform;

    short mustExecute() const override;
    /// updates the Placement property from the Placement of the BaseFeature
    void positionByBaseFeature();
    /**
     * Returns the BaseFeature property's object if it's set otherwise returns Base's
     * feature property object otherwise feature property's object (if any)
     * @param silent if couldn't determine the base feature and silent == true,
     *               silently return a nullptr, otherwise throw Base::Exception.
     *               Default is false.
     */
    Part::Feature* getBaseObject(bool silent = false) const override;
    /// extracts all edges from the subshapes (including face edges) and furthermore adds
    /// all C0 continuous edges to the vector
    void getContinuousEdges(Part::TopoShape, std::vector<std::string>&);
    // add argument to return the selected face that edges were derived from
    void getContinuousEdges(Part::TopoShape, std::vector<std::string>&, std::vector<std::string>&);
    // Todo: Post-TNP the above two versions should be able to be factored out.
    std::vector<TopoShape> getContinuousEdges(const TopoShape& shape);

    std::vector<TopoShape> getFaces(const TopoShape& shape);
    void getAddSubShape(Part::TopoShape& addShape, Part::TopoShape& subShape) override;
    void updatePreviewShape() override;

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_DressUp_H
