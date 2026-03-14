#include "GeoEnum.h"

#include <FCConfig.h>

using namespace Sketcher;

bool GeoElementId::operator==(const GeoElementId& obj) const
{
    return this->GeoId == obj.GeoId && this->Pos == obj.Pos;
}

bool GeoElementId::operator!=(const GeoElementId& obj) const
{
    return this->GeoId != obj.GeoId || this->Pos != obj.Pos;
}

#ifdef FC_OS_WIN32
constexpr const GeoElementId GeoElementId::RtPnt = GeoElementId(GeoEnum::RtPnt, PointPos::start);
constexpr const GeoElementId GeoElementId::HAxis = GeoElementId(GeoEnum::HAxis, PointPos::none);
constexpr const GeoElementId GeoElementId::VAxis = GeoElementId(GeoEnum::VAxis, PointPos::none);
#endif
