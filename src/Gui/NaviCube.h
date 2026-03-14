#ifndef SRC_GUI_NAVICUBE_H_
#define SRC_GUI_NAVICUBE_H_

#include <CXX/Extensions.hxx>
#include <QColor>
#include <FCGlobal.h>

class SoEvent;

namespace Gui
{
class View3DInventorViewer;
}

class NaviCubeImplementation;

class GuiExport NaviCube
{
public:
    enum Corner
    {
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner
    };
    NaviCube(Gui::View3DInventorViewer* viewer);
    virtual ~NaviCube();
    void drawNaviCube();
    void createContextMenu(const std::vector<std::string>& cmd);
    bool processSoEvent(const SoEvent* ev);
    void setCorner(Corner);
    void setOffset(int x, int y);
    bool isDraggable();
    void updateColors();
    void setDraggable(bool draggable);
    void setSize(int size);
    void setChamfer(float size);
    void setNaviRotateToNearest(bool toNearest);
    void setNaviStepByTurn(int steps);
    void setFont(std::string font);
    void setFontWeight(int weight);
    void setFontStretch(int stretch);
    void setFontZoom(float zoom);
    void setBaseColor(QColor TextColor);
    void setEmphaseColor(QColor ButtonColor);
    void setHiliteColor(QColor HiliteColor);
    void setBorderWidth(double BorderWidth);
    void setShowCS(bool showCS);
    void setInactiveOpacity(float opacity);
    // Label order: front, top, right, rear, bottom, left
    void setNaviCubeLabels(const std::vector<std::string>& labels);
    static void setNaviCubeCommands(const std::vector<std::string>& cmd);
    static int getNaviCubeSize();

private:
    NaviCubeImplementation* m_NaviCubeImplementation;
};

#endif /* SRC_GUI_NAVICUBE_H_ */
