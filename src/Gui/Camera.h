#ifndef GUI_CAMERA_H
#define GUI_CAMERA_H

#include <Inventor/SbRotation.h>
#include <Base/Rotation.h>
#include <FCGlobal.h>

namespace Gui
{

class GuiExport Camera
{
public:
    enum Orientation
    {
        Top,
        Bottom,
        Front,
        Rear,
        Right,
        Left,
        Isometric,
        Dimetric,
        Trimetric,
    };

    static SbRotation top();
    static SbRotation bottom();
    static SbRotation front();
    static SbRotation rear();
    static SbRotation right();
    static SbRotation left();
    static SbRotation isometric();
    static SbRotation dimetric();
    static SbRotation trimetric();

    static SbRotation rotation(Orientation view);
    static Base::Rotation convert(Orientation view);
    static Base::Rotation convert(const SbRotation&);
    static SbRotation convert(const Base::Rotation&);
};

}  // namespace Gui

#endif  // GUI_CAMERA_H
