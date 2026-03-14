#ifndef TECHDRAW_DIMENSIONREFERENCES_H
#define TECHDRAW_DIMENSIONREFERENCES_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <string>
#include <vector>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <Mod/Part/App/TopoShape.h>
#include <Mod/TechDraw/App/Geometry.h>

namespace App
{
class DocumentObject;
class Document;
}

namespace Part
{
class TopoShape;
}

namespace TechDraw
{
class DrawViewPart;

//a convenient way of handling object+subName references
class TechDrawExport ReferenceEntry
{
public:
    ReferenceEntry() = default;
    ReferenceEntry( App::DocumentObject* docObject, const std::string& subName, App::Document* document = nullptr);
    ReferenceEntry(const ReferenceEntry& other);
    ~ReferenceEntry() = default;

    ReferenceEntry& operator= (const ReferenceEntry& otherRef);
    bool operator== (const ReferenceEntry& otherRef) const;

    App::DocumentObject* getObject() const;
    template <class T> T* getObject() const { return freecad_cast<T*>(getObject()); }
    void setObject(App::DocumentObject* docObj) { m_object = docObj; }
    std::string getSubName(bool longForm = false) const;
    void setSubName(const std::string& subName) { m_subName = subName; }
    std::string getObjectName() const { return m_objectName; }
    void setObjectName(const std::string& name) { m_objectName = name; }
    App::Document* getDocument() const { return m_document; }
    void setDocument(App::Document* document) { m_document = document; }

    TopoDS_Shape getGeometry() const;
    std::string geomType() const;
    GeomType geomEdgeType() const;

    bool isWholeObject() const;

    Part::TopoShape asTopoShape() const;
    Part::TopoShape asCanonicalTopoShape() const;
    static Part::TopoShape asCanonicalTopoShape(const Part::TopoShape& inShape, const DrawViewPart& dvp);

    bool is3d() const;
    bool hasGeometry() const;

private:
    bool hasGeometry2d() const;
    TopoDS_Shape getGeometry2d() const;

    static Part::TopoShape asTopoShapeVertex(const TopoDS_Vertex &vert);
    static Part::TopoShape asTopoShapeEdge(const TopoDS_Edge& edge);
    static Part::TopoShape asTopoShapeFace(const TopoDS_Face& edge);

    App::DocumentObject* m_object{nullptr};
    std::string m_subName;
    std::string m_objectName;
    App::Document* m_document{nullptr};
};

using ReferenceVector = std::vector<ReferenceEntry>;

} // end namespace

#endif //TECHDRAW_DIMENSIONREFERENCES_H
