#ifndef INPUTHINTWIDGET_H
#define INPUTHINTWIDGET_H

#include <optional>

#include <FCGlobal.h>

#include "StatusBarLabel.h"
#include "InputHint.h"

namespace Gui
{
class GuiExport InputHintWidget: public StatusBarLabel
{
    Q_OBJECT

public:
    explicit InputHintWidget(QWidget* parent);

    void showHints(const std::list<InputHint>& hints);
    void clearHints();

private:
    static std::optional<const char*> getCustomIconPath(InputHint::UserInput key);
    static QString inputRepresentation(InputHint::UserInput key);
    QPixmap generateKeyIcon(InputHint::UserInput key, QColor color, int height = 24);
};

}  // Namespace Gui
#endif  // INPUTHINTWIDGET_H
