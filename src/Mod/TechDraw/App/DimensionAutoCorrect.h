// a class to validate and correct dimension references

#ifndef DIMAUTOCORRECT_H
#define DIMAUTOCORRECT_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <DrawViewDimension.h>

namespace Part
{
class TopoShape;
}

namespace TechDraw
{
class GeometryMatcher;


class TechDrawExport DimensionAutoCorrect
{
public:
    DimensionAutoCorrect() = default;
    explicit DimensionAutoCorrect(DrawViewDimension* dim) :
           m_dimension(dim)  {}
    ~DimensionAutoCorrect() = default;

    bool referencesHaveValidGeometry(std::vector<bool>& referenceState) const;
    bool autocorrectReferences(std::vector<bool>& referenceState,
                               ReferenceVector& repairedRefs) const;

    void set3dObjectCache(std::set<std::string> cache)
    {
        m_3dObjectCache = cache;
    }
    bool fixBrokenReferences(ReferenceVector& fixedReferences) const;

private:
    bool fix1GeomExact(ReferenceEntry& refToFix, const TopoDS_Shape& geomToMatch) const;
    bool fix1GeomSimilar(ReferenceEntry& refToFix, const TopoDS_Shape& geomToMatch) const;

    bool findExactVertex2d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;
    bool findExactEdge2d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;
    bool findExactVertex3d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;
    bool findExactEdge3d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;

    bool findSimilarVertex2d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;
    bool findSimilarEdge2d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;
    bool findSimilarVertex3d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;
    bool findSimilarEdge3d(ReferenceEntry& refToFix, const Part::TopoShape& refGeom) const;

    ReferenceEntry
    searchObjForVert(App::DocumentObject* obj, const Part::TopoShape& refVertex, bool exact = true) const;
    ReferenceEntry
    searchViewForVert(DrawViewPart* obj, const Part::TopoShape& refVertex, bool exact = true) const;
    ReferenceEntry
    searchObjForEdge(App::DocumentObject* obj, const Part::TopoShape& refEdge, bool exact = true) const;

    ReferenceEntry searchViewForExactEdge(DrawViewPart* obj, const Part::TopoShape& refEdge) const;
    ReferenceEntry searchViewForSimilarEdge(DrawViewPart* obj, const Part::TopoShape& refEdge) const;

    bool isMatchingGeometry(const ReferenceEntry& ref, const Part::TopoShape& savedGeometry) const;

    DrawViewDimension* getDimension() const
    {
        return m_dimension;
    }
    GeometryMatcher* getMatcher() const;

    DrawViewDimension* m_dimension{nullptr};
    std::set<std::string> m_3dObjectCache;
};

}  // end namespace TechDraw
#endif
