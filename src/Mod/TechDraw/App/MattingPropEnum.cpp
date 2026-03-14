#include "MattingPropEnum.h"


namespace TechDraw {

const int   MattingPropEnum::MattingCount = 2;
const char* MattingPropEnum::MattingTypeEnums[]= {
    QT_TRANSLATE_NOOP("MattingPropEnum", "Circle"),
    QT_TRANSLATE_NOOP("MattingPropEnum", "Square"),
    nullptr};

const std::vector<std::string> MattingPropEnum::MattingTypeIcons = { ":icons/circular.svg",
                                            ":icons/square.svg"};

}
