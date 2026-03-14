#ifndef ARROWENUMS_H_
#define ARROWENUMS_H_

#include <string>
#include <vector>
#include <QCoreApplication>

#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDraw
{

//common definitions for line ends / arrows
enum class ArrowType : int {
    FILLED_ARROW = 0,
    OPEN_ARROW,
    TICK,
    DOT,
    OPEN_CIRCLE,
    FORK,
    FILLED_TRIANGLE,
    NONE
};

class TechDrawExport ArrowPropEnum {
    Q_DECLARE_TR_FUNCTIONS(TechDraw::ArrowPropEnum)

    public:
        static const char* ArrowTypeEnums[];
        static const int   ArrowCount;
        static const std::vector<std::string> ArrowTypeIcons;

private:

};

} //end namespace TechDraw
#endif
