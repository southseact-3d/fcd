#ifndef GUI_MULTISAMPLE_H
#define GUI_MULTISAMPLE_H

#include <map>
#include <vector>
#include <FCGlobal.h>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QSurfaceFormat>

namespace Gui
{

/** @name Anti-Aliasing modes of the rendered 3D scene
 * Specifies Anti-Aliasing (AA) method
 * - Smoothing enables OpenGL line and vertex smoothing (basically deprecated)
 * - MSAA is hardware multi sampling (with 2, 4, 6 or 8 passes), a quite common and efficient AA
 * technique
 */
//@{
enum class AntiAliasing
{
    None = 0,
    MSAA1x = 1,
    MSAA2x = 2,
    MSAA4x = 3,
    MSAA6x = 5,
    MSAA8x = 4
};
//@}

class GuiExport Multisample
{
public:
    Multisample();
    bool testSamples(int num) const;
    std::vector<std::pair<QString, AntiAliasing>> supported() const;
    static int toSamples(AntiAliasing msaa);
    static AntiAliasing toAntiAliasing(int samples);
    static AntiAliasing readMSAAFromSettings();
    static void writeMSAAToSettings(AntiAliasing msaa);

private:
    QSurfaceFormat format;
    QOpenGLContext context;
    QOffscreenSurface offscreen;
};

}  // namespace Gui

#endif  // GUI_MULTISAMPLE_H
