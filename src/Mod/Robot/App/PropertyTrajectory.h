#ifndef PROPERTYTRAJECTORY_H
#define PROPERTYTRAJECTORY_H

#include <App/Property.h>
#include <Base/BoundBox.h>

#include <Mod/Robot/RobotGlobal.h>

#include "Trajectory.h"


namespace Robot
{

/** The part shape property class.
 * @author Werner Mayer
 */
class RobotExport PropertyTrajectory: public App::Property
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyTrajectory();
    ~PropertyTrajectory() override;

    /** @name Getter/setter */
    //@{
    /// set the part shape
    void setValue(const Trajectory&);
    /// get the part shape
    const Trajectory& getValue() const;
    //@}


    /** @name Getting basic geometric entities */
    //@{
    /** Returns the bounding box around the underlying mesh kernel */
    Base::BoundBox3d getBoundingBox() const;
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

    App::Property* Copy() const override;
    void Paste(const App::Property& from) override;
    unsigned int getMemSize() const override;
    //@}

private:
    Trajectory _Trajectory;
};


}  // namespace Robot


#endif  // PROPERTYTOPOSHAPE_H
