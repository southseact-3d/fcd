#ifndef __stock_object_h__
#define __stock_object_h__
#include "SolidObject.h"
#include "linmath.h"

namespace MillSim
{

class StockObject: public SolidObject
{
public:
    StockObject();

    void GenerateBoxStock(float x, float y, float z, float l, float w, float h);

private:
    float mProfile[8] = {};
};
}  // namespace MillSim

#endif
