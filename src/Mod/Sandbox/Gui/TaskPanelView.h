#ifndef SANDBOXGUI_TASKPANELVIEW_H
#define SANDBOXGUI_TASKPANELVIEW_H

#include <QWidget>

namespace SandboxGui {

/** A test class. A more elaborate class description.
 */
class TaskPanelView : public QWidget
{
    //Q_OBJECT

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    TaskPanelView(QWidget *parent=0);

    /**
     * A destructor.
     * A more elaborate description of the destructor.
    */
    virtual ~TaskPanelView();

    void executeAction();
    void on_rbDefaultScheme_toggled(bool b);
    void on_rbXPBlueScheme_toggled(bool b);
    void on_rbXPBlue2Scheme_toggled(bool b);
    void on_rbVistaScheme_toggled(bool b);
    void on_rbMacScheme_toggled(bool b);
    void on_rbAndroidScheme_toggled(bool b);

private:
    QWidget* actionGroup;
};

} // namespace SandboxGui

#endif // SANDBOXGUI_TASKPANELVIEW_H
