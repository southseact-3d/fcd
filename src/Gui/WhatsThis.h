#ifndef GUI_WHATSTHIS_H
#define GUI_WHATSTHIS_H

#include <QString>
#include "Command.h"

namespace Gui
{

/**
 * @author Werner Mayer
 */
class StdCmdDescription: public Command
{
public:
    StdCmdDescription();
    ~StdCmdDescription() override;

    static bool inDescriptionMode();
    static void enterDescriptionMode();
    static void leaveDescriptionMode();

    static void setSource(const QString&);

protected:
    Action* createAction() override;
    void activated(int iMsg) override;

private:
    static bool _descrMode;
};

}  // namespace Gui

#endif  // GUI_WHATSTHIS_H
