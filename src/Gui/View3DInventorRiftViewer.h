#ifndef GUI_View3DInventorRiftViewer_H
#define GUI_View3DInventorRiftViewer_H

#if BUILD_VR

# include "CoinRiftWidget.h"

namespace Gui
{

class View3DInventorRiftViewer: public CoinRiftWidget
{
public:
    View3DInventorRiftViewer();
    ~View3DInventorRiftViewer();

    virtual void setSceneGraph(SoNode* sceneGraph);

protected:
    SoGroup* workplace;
    SoTranslation* translation;
    SoRotationXYZ* rotation1;
    SoRotationXYZ* rotation2;
    SoScale* scale;

protected:
    void keyPressEvent(QKeyEvent*);
};


}  // namespace Gui

#endif  // BUILD_VR

#endif  // GUI_View3DInventorRiftViewer_H
