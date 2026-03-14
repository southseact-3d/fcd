#include <Base/Console.h>
#include <Gui/MainWindow.h>
#include <Gui/MDIView.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "Grabber3d.h"

using namespace TechDrawGui;
using namespace Gui;

void Grabber3d::quickView(View3DInventor* view3d,
                          const QColor bgColor,
                          QImage &image)
{
//    Base::Console().message("G3d::quickView());
    if (!Gui::getMainWindow()) {
        //this should already be checked in the caller
        Base::Console().warning("G3d::quickView - no Main Window - returning\n");
        return;
    }

    if (!view3d) {
        //this should also already be checked in the caller
        Base::Console().warning("G3d::quickView - no 3D view for ActiveView - returning\n");
        return;
    }

    View3DInventorViewer* viewer = view3d->getViewer();
    if (!viewer) {
        Base::Console().warning("G3d::quickView - could not create viewer - returning\n");
        return;
    }

    int samples = 8;  //magic number from Gui::View3DInventorViewer
    viewer->savePicture(image.width(), image.height(), samples, bgColor, image);
}

