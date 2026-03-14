#ifndef GUI_NAVIGATIONANIMATOR_H
#define GUI_NAVIGATIONANIMATOR_H

#include "Navigation/NavigationStyle.h"
#include <QObject>
#include <memory>

namespace Gui
{

class NavigationAnimation;

class GuiExport NavigationAnimator: public QObject
{
    Q_OBJECT
public:
    NavigationAnimator();
    ~NavigationAnimator();
    void start(const std::shared_ptr<NavigationAnimation>& animation);
    bool startAndWait(const std::shared_ptr<NavigationAnimation>& animation);
    void stop();
    bool isAnimating() const;

private:
    std::shared_ptr<NavigationAnimation> activeAnimation;
};

}  // namespace Gui

#endif  // GUI_NAVIGATIONANIMATOR_H
