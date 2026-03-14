//! MeasureInfo.h
//! ancestor class and the various flavours of MeasureXXXXXInfo.

#ifndef PART_MEASUREINFO_H
#define PART_MEASUREINFO_H

#include <Mod/Part/PartGlobal.h>

#include <functional>
#include <string>
#include <vector>
#include <memory>

#include <App/DocumentObserver.h>
#include <Base/Vector3D.h>
#include <Base/Placement.h>
#include <TopoDS_Shape.hxx>

namespace Part
{

class MeasureInfo;
using MeasureInfoPtr = std::shared_ptr<MeasureInfo>;
class MeasureAngleInfo;
using MeasureAngleInfoPtr = std::shared_ptr<MeasureAngleInfo>;
class MeasureAreaInfo;
using MeasureAreaInfoPtr = std::shared_ptr<MeasureAreaInfo>;
class MeasureDistanceInfo;
using MeasureDistanceInfoPtr = std::shared_ptr<MeasureDistanceInfo>;
class MeasureLengthInfo;
using MeasureLengthInfoPtr = std::shared_ptr<MeasureLengthInfo>;
class MeasurePositionInfo;
using MeasurePositionInfoPtr = std::shared_ptr<MeasurePositionInfo>;
class MeasureRadiusInfo;
using MeasureRadiusInfoPtr = std::shared_ptr<MeasureRadiusInfo>;


class PartExport MeasureInfo
{
public:
    // making the destructor virtual so MeasureInfo is polymorphic
    MeasureInfo() = default;
    MeasureInfo(bool val)
        : valid(val)
    {}
    virtual ~MeasureInfo() = default;
    bool valid {false};
};

class PartExport MeasureAngleInfo: public MeasureInfo
{
public:
    MeasureAngleInfo() = default;
    MeasureAngleInfo(bool val, Base::Vector3d orient, Base::Vector3d pos)
        : MeasureInfo(val)
        , orientation(orient)
        , position(pos)
    {}
    ~MeasureAngleInfo() override = default;

    Base::Vector3d orientation {0.0, 0.0, 0.0};
    Base::Vector3d position {0.0, 0.0, 0.0};
};

class PartExport MeasureAreaInfo: public MeasureInfo
{
public:
    MeasureAreaInfo() = default;
    MeasureAreaInfo(bool val, double a2, Base::Placement plm)
        : MeasureInfo(val)
        , area(a2)
        , placement(plm)
    {}
    ~MeasureAreaInfo() override = default;

    double area {0};
    Base::Placement placement {};
};

// Translate geometry reference into an OCC type
class PartExport MeasureDistanceInfo: public MeasureInfo
{
public:
    MeasureDistanceInfo() = default;
    explicit MeasureDistanceInfo(bool val, const TopoDS_Shape& shp)
        : MeasureInfo(val)
        , shape(shp)
    {}
    ~MeasureDistanceInfo() override = default;

    const TopoDS_Shape& getShape()
    {
        return shape;
    }

private:
    TopoDS_Shape shape;
};

class PartExport MeasureLengthInfo: public MeasureInfo
{
public:
    MeasureLengthInfo() = default;
    MeasureLengthInfo(bool val, double len, Base::Placement plm)
        : MeasureInfo(val)
        , length(len)
        , placement(plm)
    {}
    ~MeasureLengthInfo() override = default;

    double length {0};
    Base::Placement placement {};
};

class PartExport MeasurePositionInfo: public MeasureInfo
{
public:
    MeasurePositionInfo() = default;
    MeasurePositionInfo(bool val, Base::Vector3d pos)
        : MeasureInfo(val)
        , position(pos)
    {}
    ~MeasurePositionInfo() override = default;

    Base::Vector3d position {0.0, 0.0, 0.0};
};

class PartExport MeasureRadiusInfo: public MeasureInfo
{
public:
    MeasureRadiusInfo() = default;
    MeasureRadiusInfo(bool val, double rad, Base::Vector3d point, Base::Placement plm)
        : MeasureInfo(val)
        , radius(rad)
        , pointOnCurve(point)
        , placement(plm)
    {}
    ~MeasureRadiusInfo() override = default;

    double radius {};
    Base::Vector3d pointOnCurve;
    Base::Placement placement;  // curve center & circle orientation
};

//! callback registrations
// TODO: is there more that one place that GeometryHandler is defined?
using GeometryHandler = std::function<Part::MeasureInfoPtr(App::SubObjectT)>;

class PartExport CallbackRegistrationRecord
{
public:
    CallbackRegistrationRecord() = default;
    CallbackRegistrationRecord(
        const std::string& module,
        const std::string& measureType,
        GeometryHandler callback
    )
        : m_module(module)
        , m_measureType(measureType)
        , m_callback(callback)
    {}

    std::string m_module;
    std::string m_measureType;
    GeometryHandler m_callback;
};

using CallbackRegistrationList = std::vector<CallbackRegistrationRecord>;

}  // end namespace Part

#endif
