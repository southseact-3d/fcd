#ifndef MATTINGENUMS_H_
#define MATTINGENUMS_H_

#include <string>
#include <vector>
#include <QCoreApplication>

#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDraw
{

class TechDrawExport MattingPropEnum {
    Q_DECLARE_TR_FUNCTIONS(TechDraw::MattingPropEnum)

    public:
        static const char* MattingTypeEnums[];
        static const int   MattingCount;
        static const std::vector<std::string> MattingTypeIcons;

private:

};

} //end namespace TechDraw
#endif
