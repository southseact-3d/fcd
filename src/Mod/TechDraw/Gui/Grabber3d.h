#ifndef Grabber3d_h_
#define Grabber3d_h_

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>
#include <QImage>

namespace Gui {
class View3DInventor;
}

namespace App {
class Document;
}

namespace TechDrawGui
{

/// Utility functions for obtaining 3d window image
class TechDrawGuiExport Grabber3d {
public:
    static void quickView(Gui::View3DInventor* view3d,
                          const QColor bgColor,
                          QImage &image);
};

} //end namespace TechDrawGui
#endif
