#ifndef POINTS_PROPERTYPOINTKERNEL_H
#define POINTS_PROPERTYPOINTKERNEL_H

#include "Points.h"

namespace Points
{

/** The point kernel property
 */
class PointsExport PropertyPointKernel: public App::PropertyComplexGeoData
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyPointKernel();

    /** @name Getter/setter */
    //@{
    /// Sets the points to the property
    void setValue(const PointKernel& m);
    /// get the points (only const possible!)
    const PointKernel& getValue() const;
    const Data::ComplexGeoData* getComplexData() const override;
    void setTransform(const Base::Matrix4D& rclTrf) override;
    Base::Matrix4D getTransform() const override;
    //@}

    /** @name Getting basic geometric entities */
    //@{
    /** Returns the bounding box around the underlying mesh kernel */
    Base::BoundBox3d getBoundingBox() const override;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject() override;
    void setPyObject(PyObject* value) override;
    //@}

    /** @name Undo/Redo */
    //@{
    /// returns a new copy of the property (mainly for Undo/Redo and transactions)
    App::Property* Copy() const override;
    /// paste the value from the property (mainly for Undo/Redo and transactions)
    void Paste(const App::Property& from) override;
    unsigned int getMemSize() const override;
    //@}

    /** @name Save/restore */
    //@{
    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;
    void SaveDocFile(Base::Writer& writer) const override;
    void RestoreDocFile(Base::Reader& reader) override;
    //@}

    /** @name Modification */
    //@{
    PointKernel* startEditing();
    void finishEditing();
    /// Transform the real 3d point kernel
    void transformGeometry(const Base::Matrix4D& rclMat) override;
    void removeIndices(const std::vector<unsigned long>&);
    //@}

private:
    Base::Reference<PointKernel> _cPoints;
};

}  // namespace Points


#endif  // POINTS_PROPERTYPOINTKERNEL_H
