#ifndef PART_DATUMS_H
#define PART_DATUMS_H

#include <App/Datums.h>

#include <Mod/Part/PartGlobal.h>

#include "AttachExtension.h"

namespace Part
{

class PartExport DatumPlane: public App::Plane, public AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Part::DatumPlane);

public:
    DatumPlane();
    ~DatumPlane() override = default;
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderPlane";
    }
};

class PartExport DatumLine: public App::Line, public AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Part::DatumLine);

public:
    DatumLine();
    ~DatumLine() override = default;
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderLine";
    }
};

class PartExport DatumPoint: public App::Point, public AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Part::DatumPoint);

public:
    DatumPoint();
    ~DatumPoint() override = default;
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderPoint";
    }
};

class PartExport LocalCoordinateSystem: public App::LocalCoordinateSystem, public AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Part::LocalCoordinateSystem);

public:
    LocalCoordinateSystem();
    ~LocalCoordinateSystem() override = default;
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderLCS";
    }
};

}  // namespace Part


#endif  // PART_DATUMS_H
