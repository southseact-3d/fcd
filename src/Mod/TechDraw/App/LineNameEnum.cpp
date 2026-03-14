#include "Preferences.h"

#include "LineNameEnum.h"


namespace TechDraw {

// these strings have to be kept in order according to Preferences::lineStandard()
std::vector<std::string> LineName::ContextStrings{
                            "ANSILineTypeEnum",
                            "ASMELineTypeEnum",
                            "ISOLineTypeEnum" };

std::string LineName::translationContext(size_t iStandard)
{
    if (iStandard < ContextStrings.size() &&
        iStandard > 0) {
        return ContextStrings[iStandard];
    }
    return {};
}


std::string LineName::currentTranslationContext()
{
    return translationContext(Preferences::lineStandard());
}



//! line numbers begin at 1, not 0

const int   ISOLineName::ISOLineNameCount = 15;
const char* ISOLineName::ISOLineNameEnums[]= {
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "NoLine"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "Continuous"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "Dashed"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DashedSpaced"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "LongDashedDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "LongDashedDoubleDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "LongDashedTripleDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "Dotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "LongDashShortDash"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "LongDashDoubleShortDash"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DashedDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DoubleDashedDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DashedDoubleDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DoubleDashedDoubleDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DashedTripleDotted"),
        QT_TRANSLATE_NOOP("ISOLineTypeEnum", "DoubleDashedTripleDotted"),
            nullptr};


const int   ANSILineName::ANSILineNameCount = 4;
const char* ANSILineName::ANSILineNameEnums[]= {
        QT_TRANSLATE_NOOP("ANSILineTypeEnum", "NoLine"),
        QT_TRANSLATE_NOOP("ANSILineTypeEnum", "Continuous"),
        QT_TRANSLATE_NOOP("ANSILineTypeEnum", "Dashed"),
        QT_TRANSLATE_NOOP("ANSILineTypeEnum", "LongDashDashed"),
        QT_TRANSLATE_NOOP("ANSILineTypeEnum", "LongDashDoubleDashed"),
           nullptr};


const int   ASMELineName::ASMELineNameCount = 18;
const char* ASMELineName::ASMELineNameEnums[]= {
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "NoLine"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Visible"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Hidden"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Section"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Center"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Symmetry"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Dimension"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Extension"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Leader"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "CuttingPlane"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "ViewingPlane"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "OtherPlane"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Break1"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Break2"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Phantom"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Stitch1"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Stitch2"),
            QT_TRANSLATE_NOOP("ASMELineTypeEnum", "Chain"),
           nullptr};

}

