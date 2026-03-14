#ifndef BALLOONENUMS_H_
#define BALLOONENUMS_H_

#include <string>
#include <vector>
#include <QCoreApplication>

#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDraw
{

enum class BalloonType {
    Circular = 0,
    None,
    Triangle,
    Inspection,
    Hexagon,
    Square,
    Rectangle,
    Line
};

class TechDrawExport BalloonPropEnum {
    Q_DECLARE_TR_FUNCTIONS(TechDraw::BalloonPropEnum)

    public:
        static const char* BalloonTypeEnums[];
        static const int   BalloonCount;
        static const std::vector<std::string> BalloonTypeIcons;

private:

};

} //end namespace TechDraw
#endif
