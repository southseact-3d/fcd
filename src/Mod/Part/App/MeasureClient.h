#ifndef PART_MEASURE_H
#define PART_MEASURE_H

#include <Mod/Part/PartGlobal.h>

#include "MeasureInfo.h"


namespace Part
{


class PartExport MeasureClient
{
public:
    static void initialize();

    static CallbackRegistrationList reportLengthCB();
    static CallbackRegistrationList reportPositionCB();
    static CallbackRegistrationList reportAreaCB();
    static CallbackRegistrationList reportAngleCB();
    static CallbackRegistrationList reportDistanceCB();
    static CallbackRegistrationList reportRadiusCB();
};


}  // namespace Part

#endif
