#include "NavigationAnimator.h"
#include "NavigationAnimation.h"
#include <QEventLoop>

using namespace Gui;

NavigationAnimator::NavigationAnimator()
    : activeAnimation(nullptr)
{}

NavigationAnimator::~NavigationAnimator()
{
    stop();
}

/**
 * @brief Start an animation
 *
 * @param animation The animation to start
 */
void NavigationAnimator::start(const std::shared_ptr<NavigationAnimation>& animation)
{
    stop();
    activeAnimation = animation;
    activeAnimation->initialize();

    connect(activeAnimation.get(), &NavigationAnimation::finished, this, [this]() {
        activeAnimation->onStop(true);
        activeAnimation.reset();
    });

    activeAnimation->start();
}

/**
 * @brief Start an animation and wait for it to finish
 *
 * @param animation The animation to start
 * @return True if the animation finished, false if interrupted
 */
bool NavigationAnimator::startAndWait(const std::shared_ptr<NavigationAnimation>& animation)
{
    stop();
    bool finished = true;
    QEventLoop loop;
    connect(animation.get(), &NavigationAnimation::finished, &loop, &QEventLoop::quit);
    connect(animation.get(), &NavigationAnimation::interrupted, &loop, [&finished, &loop]() {
        finished = false;
        loop.quit();
    });
    start(animation);
    loop.exec();
    return finished;
}

/**
 * @brief Stops an active animation and releases shared ownership of the animation
 */
void NavigationAnimator::stop()
{
    if (activeAnimation && activeAnimation->state() != QAbstractAnimation::State::Stopped) {
        disconnect(activeAnimation.get(), &NavigationAnimation::finished, 0, 0);
        Q_EMIT activeAnimation->interrupted();
        activeAnimation->stop();
        activeAnimation->onStop(false);
        activeAnimation.reset();
    }
}

/**
 * @return Whether or not an animation is active
 */
bool NavigationAnimator::isAnimating() const
{
    if (activeAnimation != nullptr) {
        return activeAnimation->state() == QAbstractAnimation::State::Running;
    }

    return false;
}
