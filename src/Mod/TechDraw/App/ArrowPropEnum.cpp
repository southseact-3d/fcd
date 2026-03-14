#include "ArrowPropEnum.h"


namespace TechDraw {

const int   ArrowPropEnum::ArrowCount = 8;
const char* ArrowPropEnum::ArrowTypeEnums[]= {
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Filled arrow"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Open arrow"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Tick"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Dot"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Open circle"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Fork"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "Filled triangle"),
    QT_TRANSLATE_NOOP("ArrowPropEnum", "None"),
    nullptr};

const std::vector<std::string> ArrowPropEnum::ArrowTypeIcons = { ":icons/arrowfilled.svg",
                                            ":icons/arrowopen.svg",
                                            ":icons/arrowtick.svg",
                                            ":icons/arrowdot.svg",
                                            ":icons/arrowopendot.svg",
                                            ":icons/arrowfork.svg",
                                            ":icons/arrowpyramid.svg",
                                            ":icons/arrownone.svg"};

}
