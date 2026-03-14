#ifndef Fem_PropertyFemMesh_H
#define Fem_PropertyFemMesh_H

#include "FemMesh.h"
#include <App/PropertyGeo.h>
#include <Base/BoundBox.h>

namespace Fem
{


/** The part shape property class.
 * @author Werner Mayer
 */
class FemExport PropertyFemMesh: public App::PropertyComplexGeoData
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyFemMesh();
    ~PropertyFemMesh() override;

    /** @name Getter/setter */
    //@{
    void setValuePtr(FemMesh* mesh);
    /// set the FemMesh shape
    void setValue(const FemMesh&);
    /// does nothing, for add property macro
    void setValue()
    {}
    /// get the FemMesh shape
    const FemMesh& getValue() const;
    const Data::ComplexGeoData* getComplexData() const override;
    //@}


    /** @name Getting basic geometric entities */
    //@{
    /** Returns the bounding box around the underlying mesh kernel */
    Base::BoundBox3d getBoundingBox() const override;
    /// Set the placement of the geometry
    void setTransform(const Base::Matrix4D& rclTrf) override;
    /// Get the placement of the geometry
    Base::Matrix4D getTransform() const override;
    void transformGeometry(const Base::Matrix4D& rclMat) override;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject() override;
    void setPyObject(PyObject* value) override;
    //@}

    /** @name Save/restore */
    //@{
    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;
    void SaveDocFile(Base::Writer& writer) const override;
    void RestoreDocFile(Base::Reader& reader) override;

    App::Property* Copy() const override;
    void Paste(const App::Property& from) override;
    unsigned int getMemSize() const override;
    const char* getEditorName() const override
    {
        return "FemGui::PropertyFemMeshItem";
    }
    //@}

private:
    Base::Reference<FemMesh> _FemMesh;
};


}  // namespace Fem


#endif  // PROPERTYTOPOSHAPE_H
