#ifndef TECHDRAWGUI_USERTYPES_H
#define TECHDRAWGUI_USERTYPES_H

#include <QGraphicsItem>

/*
Derived QGI Classes type() Values

Qt First UserType>> QGraphicsItem::UserType = 65536
*/

namespace TechDrawGui {
struct UserType {
enum : int {
    QGCustomBorder = QGraphicsItem::UserType,
    QGCustomClip,
    QGCustomImage,
    QGCustomLabel,
    QGCustomRect,
    QGCustomSvg,
    QGCustomText,
    QGDisplayArea,
    QGEPath,
    QGIArrow,
    QGIBalloonLabel,
    QGIBreakLine,
    QGICaption,
    QGICenterLine,
    QGICMark,
    QGIDatumLabel,
    QGIDecoration,
    QGIDimLines,
    QGIDrawingTemplate,
    QGIEdge,
    QGIFace,
    QGIGhostHighlight,
    //QGIHatch,  //obsolete
    QGIHighlight,
    QGILeaderLine,
    QGIMatting,
    QGIPrimPath,
    QGIProjGroup,
    QGIRichAnno,
    QGISectionLine,
    QGISpreadsheet,
    QGISVGTemplate,
    QGITemplate,
    QGITile,
    QGIVertex,
    QGIView,
    QGIViewAnnotation,
    QGIViewBalloon,
    QGIViewClip,
    QGIViewCollection,
    QGIViewDimension,
    QGIViewImage,
    QGIViewPart,
    QGIViewSection,
    QGIViewSymbol,
    QGIWeldSymbol,
    QGMarker,
    QGMText,
    QGTracker,
    TemplateTextField
};
};
}

/*
Standard Types
path 2
rect 3
ellipse 4
polygon 5
line 6
pixmap 7
text 8
simpletext 9
group 10
*/

#endif
