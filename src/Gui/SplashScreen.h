#ifndef GUI_SPLASHSCREEN_H
#define GUI_SPLASHSCREEN_H

#include <QSplashScreen>

namespace Gui
{

class SplashObserver;

/** This widget provides a splash screen that can be shown during application startup.
 *
 * \author Werner Mayer
 */
class SplashScreen: public QSplashScreen
{
    Q_OBJECT

public:
    explicit SplashScreen(const QPixmap& pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags());
    ~SplashScreen() override;

    void setShowMessages(bool on);

    static QPixmap splashImage();

protected:
    void drawContents(QPainter* painter) override;

private:
    SplashObserver* messages;
};

}  // namespace Gui

#endif  // GUI_SPLASHSCREEN_H
