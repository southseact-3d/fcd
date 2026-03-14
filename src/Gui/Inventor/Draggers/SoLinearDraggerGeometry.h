#ifndef GUI_LINEAR_DRAGGER_GEOMETRY_H
#define GUI_LINEAR_DRAGGER_GEOMETRY_H

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodekits/SoBaseKit.h>

#include <Base/Vector3D.h>

#include <FCGlobal.h>

namespace Gui
{

class GuiExport SoLinearGeometryKit: public SoBaseKit
{
    SO_KIT_HEADER(SoLinearGeometryKit);

public:
    static void initClass();

    SoSFVec3f tipPosition;

protected:
    SoLinearGeometryKit();
    ~SoLinearGeometryKit() override = default;

private:
    using inherited = SoBaseKit;
};

class GuiExport SoArrowGeometry: public SoLinearGeometryKit
{
    SO_KIT_HEADER(SoArrowGeometry);
    SO_KIT_CATALOG_ENTRY_HEADER(separator);
    SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(arrowBody);
    SO_KIT_CATALOG_ENTRY_HEADER(arrowTip);

    SO_KIT_CATALOG_ENTRY_HEADER(_arrowBodyTranslation);
    SO_KIT_CATALOG_ENTRY_HEADER(_arrowTipTranslation);

public:
    static void initClass();
    SoArrowGeometry();

    SoSFFloat coneBottomRadius;
    SoSFFloat coneHeight;
    SoSFFloat cylinderHeight;
    SoSFFloat cylinderRadius;

protected:
    ~SoArrowGeometry() override = default;

    void notify(SoNotList* notList) override;

private:
    using inherited = SoLinearGeometryKit;
};

class GuiExport SoLinearGeometryBaseKit: public SoBaseKit
{
    SO_KIT_HEADER(SoLinearGeometryBaseKit);

public:
    static void initClass();

    SoSFVec3f translation;    //!< set from the parent dragger
    SoSFVec3f geometryScale;  //!< set from the parent dragger
    SoSFBool active;          //!< set from the parent dragger

protected:
    SoLinearGeometryBaseKit();
    ~SoLinearGeometryBaseKit() override = default;

private:
    using inherited = SoBaseKit;
};

class GuiExport SoArrowBase: public SoLinearGeometryBaseKit
{
    SO_KIT_HEADER(SoArrowBase);
    SO_KIT_CATALOG_ENTRY_HEADER(separator);
    SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(baseColor);
    SO_KIT_CATALOG_ENTRY_HEADER(cylinder);

    SO_KIT_CATALOG_ENTRY_HEADER(_cylinderTranslation);

public:
    static void initClass();
    SoArrowBase();

    SoSFFloat cylinderHeight;
    SoSFFloat cylinderRadius;
    SoSFColor color;

protected:
    ~SoArrowBase() override = default;

    void notify(SoNotList* notList) override;

private:
    using inherited = SoLinearGeometryBaseKit;
};

}  // namespace Gui

#endif /* GUI_LINEAR_DRAGGER_GEOMETRY_H */
