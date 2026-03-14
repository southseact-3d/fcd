#ifndef SKETCHERGUI_SHORTCUTLISTENER_H
#define SKETCHERGUI_SHORTCUTLISTENER_H

#include <QObject>


namespace SketcherGui
{

class ViewProviderSketch;

class ShortcutListener: public QObject
{
    // Q_OBJECT

public:
    explicit ShortcutListener(ViewProviderSketch* vp);
    ~ShortcutListener() override;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

    ViewProviderSketch* pViewProvider;
};

}  // namespace SketcherGui


#endif  // SKETCHERGUI_SHORTCUTLISTENER_H
