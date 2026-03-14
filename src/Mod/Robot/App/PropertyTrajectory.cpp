#include <sstream>


#include <Base/Reader.h>
#include <Base/Writer.h>

#include "PropertyTrajectory.h"
#include "TrajectoryPy.h"


using namespace Robot;

TYPESYSTEM_SOURCE(Robot::PropertyTrajectory, App::Property)

PropertyTrajectory::PropertyTrajectory() = default;

PropertyTrajectory::~PropertyTrajectory() = default;

void PropertyTrajectory::setValue(const Trajectory& sh)
{
    aboutToSetValue();
    _Trajectory = sh;
    hasSetValue();
}


const Trajectory& PropertyTrajectory::getValue() const
{
    return _Trajectory;
}


Base::BoundBox3d PropertyTrajectory::getBoundingBox() const
{
    Base::BoundBox3d box;
    // if (_Trajectory._Trajectory.IsNull())
    //     return box;
    // try {
    //     // If the shape is empty an exception may be thrown
    //     Bnd_Box bounds;
    //     BRepBndLib::Add(_Trajectory._Trajectory, bounds);
    //     bounds.SetGap(0.0);
    //     Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    //     bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);

    //    box.MinX = xMin;
    //    box.MaxX = xMax;
    //    box.MinY = yMin;
    //    box.MaxY = yMax;
    //    box.MinZ = zMin;
    //    box.MaxZ = zMax;
    //}
    // catch (Standard_Failure& e) {
    //}

    return box;
}


PyObject* PropertyTrajectory::getPyObject()
{
    return new TrajectoryPy(new Trajectory(_Trajectory));
}

void PropertyTrajectory::setPyObject(PyObject* value)
{
    if (PyObject_TypeCheck(value, &(TrajectoryPy::Type))) {
        TrajectoryPy* pcObject = static_cast<TrajectoryPy*>(value);
        setValue(*pcObject->getTrajectoryPtr());
    }
    else {
        std::string error = std::string("type must be 'Trajectory', not ");
        error += value->ob_type->tp_name;
        throw Base::TypeError(error);
    }
}

App::Property* PropertyTrajectory::Copy() const
{
    PropertyTrajectory* prop = new PropertyTrajectory();
    prop->_Trajectory = this->_Trajectory;

    return prop;
}

void PropertyTrajectory::Paste(const App::Property& from)
{
    aboutToSetValue();
    _Trajectory = dynamic_cast<const PropertyTrajectory&>(from)._Trajectory;
    hasSetValue();
}

unsigned int PropertyTrajectory::getMemSize() const
{
    return _Trajectory.getMemSize();
}

void PropertyTrajectory::Save(Base::Writer& writer) const
{
    _Trajectory.Save(writer);
}

void PropertyTrajectory::Restore(Base::XMLReader& reader)
{
    Robot::Trajectory temp;
    temp.Restore(reader);
    setValue(temp);
}
