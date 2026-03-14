#include "BalloonPropEnum.h"


namespace TechDraw {

const int   BalloonPropEnum::BalloonCount = 8;
const char* BalloonPropEnum::BalloonTypeEnums[]= {
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Circular"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "None"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Triangle"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Inspection"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Hexagon"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Square"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Rectangle"),
    QT_TRANSLATE_NOOP("BalloonPropEnum", "Line"),
    nullptr};

const std::vector<std::string> BalloonPropEnum::BalloonTypeIcons = { ":icons/circular.svg",
                                            ":icons/none.svg",
                                            ":icons/triangle.svg",
                                            ":icons/inspection.svg",
                                            ":icons/hexagon.svg",
                                            ":icons/square.svg",
                                            ":icons/rectangle.svg",
                                            ":icons/bottomline.svg"};

}
