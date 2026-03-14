#ifndef QUARTER_CONTEXTMENUHANDLERP_H
#define QUARTER_CONTEXTMENUHANDLERP_H

#include <QObject>

class QMenu;
class QAction;

namespace SIM { namespace Coin3D { namespace Quarter {

class QuarterWidget;

class ContextMenu : public QObject {
  Q_OBJECT
public:
  explicit ContextMenu(QuarterWidget * quarterwidget);
  ~ContextMenu() override;

  QMenu * getMenu() const;

public Q_SLOTS:
  void changeRenderMode(QAction * action);
  void changeStereoMode(QAction * action);
  void changeTransparencyType(QAction * action);

private:
  QuarterWidget * quarterwidget;

  QMenu * contextmenu;
  QMenu * rendermenu;
  QMenu * stereomenu;
  QMenu * functionsmenu;
  QMenu * transparencymenu;
};

}}} // namespace

#endif // QUARTER_CONTEXTMENUHANDLERP_H
