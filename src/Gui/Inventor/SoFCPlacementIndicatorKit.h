#ifndef SOFCPLACEMENTINDICATORKIT_H
#define SOFCPLACEMENTINDICATORKIT_H

#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFFloat.h>

#include <FCGlobal.h>

namespace Gui
{

class GuiExport SoFCPlacementIndicatorKit: public SoBaseKit
{
    using inherited = SoBaseKit;

    static constexpr float planeIndicatorRadius = 0.4F;
    static constexpr double planeIndicatorMargin = 0.2;
    static constexpr float planeIndicatorTransparency = 0.3F;

    static constexpr double axisMargin = 0.0;
    static constexpr float axisLengthDefault = 0.6F;
    static constexpr float axisThickness = 0.065F;

    static constexpr float arrowHeadRadius = axisThickness * 1.25F;
    static constexpr float arrowHeadHeight = arrowHeadRadius * 3.F;

    static constexpr double labelOffset = 0.4;
    static constexpr int labelFontSize = 9;

    static constexpr float scaleFactorDefault = 40.F;

    SO_KIT_HEADER(SoFCPlacementIndicatorKit);

    SO_KIT_CATALOG_ENTRY_HEADER(root);

public:
    SoFCPlacementIndicatorKit();
    static void initClass();

    void notify(SoNotList* l) override;

    // clang-format off
    enum Part
    {
        Axes            = 1 << 0,
        ArrowHeads      = 1 << 1,
        Labels          = 1 << 2,
        PlaneIndicator  = 1 << 3,
        OriginIndicator = 1 << 4,

        // common configurations
        AllParts       = Axes | OriginIndicator | Labels | ArrowHeads,
        AxisCross      = Axes | Labels | ArrowHeads
    };

    enum Axis
    {
        X       = 1 << 0,
        Y       = 1 << 1,
        Z       = 1 << 2,
        AllAxes = X | Y | Z
    };
    // clang-format on

    SoSFEnum parts;
    SoSFEnum axes;
    SoSFBool coloredAxis;
    SoSFFloat scaleFactor;
    SoSFFloat axisLength;

private:
    void recomputeGeometry();

    SoSeparator* createOriginIndicator();
    SoSeparator* createGeometry();
    SoSeparator* createAxes();
    SoSeparator* createPlaneIndicator();

    ~SoFCPlacementIndicatorKit() override;
};

}  // namespace Gui

#endif  // SOFCPLACEMENTINDICATORKIT_H
