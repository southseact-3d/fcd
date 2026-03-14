#ifndef JT_READER_TKJTREADER_H
#define JT_READER_TKJTREADER_H

#ifdef JTREADER_HAVE_TKJT
# include <JtAttribute_GeometricTransform.hxx>
# include <JtAttribute_Material.hxx>
# include <JtData_Model.hxx>
# include <JtElement_ShapeLOD_TriStripSet.hxx>
# include <JtNode_Instance.hxx>
# include <JtNode_Partition.hxx>
# include <JtNode_RangeLOD.hxx>
# include <JtNode_Shape_TriStripSet.hxx>
# include <JtNode_Shape_Vertex.hxx>
# include <TCollection_AsciiString.hxx>
# include <TCollection_ExtendedString.hxx>
# include <Base/Builder3D.h>

namespace JtReaderNS
{

class TKJtReader
{
public:
    TKJtReader();
    void open(const std::string& filename);
    void clear();
    std::string getOutput() const;

private:
    void traverseGraph(const Handle(JtNode_Base) & aNode);
    void readMaterialAttribute(const Handle(JtAttribute_Material) & aMaterial);
    void readTransformAttribute(const Handle(JtAttribute_GeometricTransform) & aTransform);
    void readShapeVertex(const Handle(JtNode_Shape_Vertex) & aShape);
    void readPartition(const Handle(JtNode_Partition) & aPart);
    void readGroup(const Handle(JtNode_Group) & aGroup);
    void readRangeLOD(const Handle(JtNode_RangeLOD) & aRangeLOD);
    void readInstance(const Handle(JtNode_Instance) & anInstance);
    void readAttributes(const JtData_Object::VectorOfObjects& attr);
    void getTriangleStripSet(const Handle(JtElement_ShapeLOD_TriStripSet) & aLOD);

private:
    TCollection_ExtendedString jtDir;
    Handle(JtNode_Partition) rootNode;
    std::stringstream result;
    Base::InventorBuilder builder;
    std::list<Base::Matrix4D> transformations;
};

};  // namespace JtReaderNS

#endif  // JTREADER_HAVE_TKJT

#endif  // JT_READER_TKJTREADER_H
