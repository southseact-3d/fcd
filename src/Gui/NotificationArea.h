#ifndef GUI_NOTIFICATIONAREA_H
#define GUI_NOTIFICATIONAREA_H

#include <QPushButton>
#include <QString>

#include <map>
#include <memory>

#include <Base/Observer.h>
#include <Base/Parameter.h>

namespace App
{
class Document;
}

namespace Gui
{

struct NotificationAreaP;

class NotificationArea: public QPushButton
{
    enum class TrayIcon
    {
        Normal,
        MissedNotifications,
    };

public:
    class ParameterObserver: public ParameterGrp::ObserverType
    {
    public:
        explicit ParameterObserver(NotificationArea* notificationarea);
        ~ParameterObserver() override;

        void OnChange(Base::Subject<const char*>& rCaller, const char* sReason) override;

    private:
        NotificationArea* notificationArea;
        ParameterGrp::handle hGrp;
        std::map<std::string, std::function<void(const std::string& string)>> parameterMap;
    };

    NotificationArea(QWidget* parent = nullptr);
    ~NotificationArea() override;

    void pushNotification(const QString& notifiername, const QString& message, Base::LogStyle level);

    bool areDeveloperWarningsActive() const;
    bool areDeveloperErrorsActive() const;

private:
    void showInNotificationArea();
    bool confirmationRequired(Base::LogStyle level);
    void showConfirmationDialog(const QString& notifiername, const QString& message);
    void slotRestoreFinished(const App::Document&);

    void mousePressEvent(QMouseEvent* e) override;

    void setIcon(TrayIcon trayIcon);

private:
    std::unique_ptr<NotificationAreaP> pImp;
};


}  // namespace Gui

#endif  // GUI_NOTIFICATIONAREA_H
