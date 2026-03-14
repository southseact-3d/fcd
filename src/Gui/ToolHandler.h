#ifndef GUI_ToolHandler_H
#define GUI_ToolHandler_H

#include <QCursor>
#include <QPixmap>

#include <Base/Parameter.h>
#include <Base/Tools2D.h>

#include "Selection/Selection.h"


namespace Gui
{
class View3DInventorViewer;
struct InputHint;

class GuiExport ToolHandler
{
public:
    ToolHandler() = default;
    virtual ~ToolHandler() = default;

    bool activate();
    virtual void deactivate();

    virtual void quit()
    {}

    /// updates the actCursor with the icon by calling getCrosshairCursorSVGName(),
    /// enabling to set data member dependent icons (i.e. for different construction methods)
    void updateCursor();

    virtual std::list<InputHint> getToolHints() const;
    void updateHint() const;

private:  // NVI
    virtual void preActivated()
    {}
    virtual void activated()
    {}
    virtual void deactivated()
    {}
    virtual void postDeactivated()
    {}

protected:  // NVI requiring base implementation
    virtual QString getCrosshairCursorSVGName() const;


protected:
    // helpers
    /**
     * Sets a cursor for 3D inventor view.
     * pixmap as a cursor image in device independent pixels.
     *
     * \param autoScale - set this to false if pixmap already scaled for HiDPI
     **/

    /** @name Icon helpers */
    //@{
    void setCursor(const QPixmap& pixmap, int x, int y, bool autoScale = true);


    void unsetCursor();

    /// restitutes the DSH cached cursor (without any tail due to autoconstraints, ...)
    void applyCursor();

    void addCursorTail(std::vector<QPixmap>& pixmaps);

    /// returns the color to be used for the crosshair (configurable as a parameter)
    unsigned long getCrosshairColor();

    /// functions to set the cursor to a given svgName (to be migrated to NVI style)

    qreal devicePixelRatio();
    //@}

    View3DInventorViewer* getViewer();

    virtual QWidget* getCursorWidget();

    virtual void setWidgetCursor(QCursor cursor);

private:
    void setSvgCursor(
        const QString& svgName,
        int x,
        int y,
        const std::map<unsigned long, unsigned long>& colorMapping
        = std::map<unsigned long, unsigned long>()
    );


    void applyCursor(QCursor& newCursor);

    void setCrosshairCursor(const QString& svgName);
    void setCrosshairCursor(const char* svgName);

protected:
    QCursor oldCursor;
    QCursor actCursor;
    QPixmap actCursorPixmap;
};


}  // namespace Gui


#endif  // GUI_ToolHandler_H
