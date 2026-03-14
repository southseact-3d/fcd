#include <FCGlobal.h>
#include <QProxyStyle>
#include <QEvent>
#include <QPushButton>

namespace Gui
{

class GuiExport FreeCADStyle: public QProxyStyle
{
    Q_OBJECT

public:
    FreeCADStyle()
        : QProxyStyle(QStringLiteral("Fusion"))
    {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace Gui

#endif  // FREECAD_FREECADSTYLE_H
