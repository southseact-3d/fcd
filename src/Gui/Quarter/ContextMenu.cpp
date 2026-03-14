#include <QActionGroup>
#include <QMenu>

#include "ContextMenu.h"
#include "QuarterWidget.h"


using namespace SIM::Coin3D::Quarter;

ContextMenu::ContextMenu(QuarterWidget * quarterwidget)
  : quarterwidget(quarterwidget)
{
  this->contextmenu = new QMenu;
  this->functionsmenu = new QMenu("Functions");
  this->rendermenu = new QMenu("Render Mode");
  this->stereomenu = new QMenu("Stereo Mode");
  this->transparencymenu = new QMenu("Transparency Type");

  this->contextmenu->addMenu(functionsmenu);
  this->contextmenu->addMenu(rendermenu);
  this->contextmenu->addMenu(stereomenu);
  this->contextmenu->addMenu(transparencymenu);

  SoRenderManager * sorendermanager = quarterwidget->getSoRenderManager();

  QActionGroup * rendermodegroup = nullptr;
  QActionGroup * stereomodegroup = nullptr;
  QActionGroup * transparencytypegroup = nullptr;

  Q_FOREACH (QAction * action, quarterwidget->renderModeActions()) {
    if (!rendermodegroup) {
      rendermodegroup = action->actionGroup();
    } else {
      assert(rendermodegroup && rendermodegroup == action->actionGroup());
    }

    int rendermode = static_cast<QuarterWidget::RenderMode>(sorendermanager->getRenderMode());
    int data = static_cast<QuarterWidget::RenderMode>(action->data().toInt());
    action->setChecked(rendermode == data);
    rendermenu->addAction(action);
  }

  Q_FOREACH (QAction * action, quarterwidget->stereoModeActions()) {
    if (!stereomodegroup) {
      stereomodegroup = action->actionGroup();
    } else {
      assert(stereomodegroup && stereomodegroup == action->actionGroup());
    }

    int stereomode = static_cast<QuarterWidget::StereoMode>(sorendermanager->getStereoMode());
    int data = static_cast<QuarterWidget::StereoMode>(action->data().toInt());
    action->setChecked(stereomode == data);
    stereomenu->addAction(action);
  }

  Q_FOREACH (QAction * action, quarterwidget->transparencyTypeActions()) {
    if (!transparencytypegroup) {
      transparencytypegroup = action->actionGroup();
    } else {
      assert(transparencytypegroup && transparencytypegroup == action->actionGroup());
    }

    SoGLRenderAction * renderaction = sorendermanager->getGLRenderAction();
    int transparencytype = static_cast<SoGLRenderAction::TransparencyType>(renderaction->getTransparencyType());
    int data = static_cast<SoGLRenderAction::TransparencyType>(action->data().toInt());
    action->setChecked(transparencytype == data);
    transparencymenu->addAction(action);
  }

  QAction * viewall = new QAction("View All", quarterwidget);
  QAction * seek = new QAction("Seek", quarterwidget);
  functionsmenu->addAction(viewall);
  functionsmenu->addAction(seek);

  QObject::connect(seek, &QAction::triggered,
                   this->quarterwidget, &QuarterWidget::seek);

  QObject::connect(viewall, &QAction::triggered,
                   this->quarterwidget, &QuarterWidget::viewAll);

  // FIXME: It would be ideal to expose these actiongroups to Qt
  // Designer and be able to connect them to the appropriate slots on
  // QuarterWidget, but this is not possible in Qt. Exposing every
  // single action is supposed to work, but it doesn't at the
  // moment. (20081215 frodo)
  QObject::connect(rendermodegroup, &QActionGroup::triggered,
                   this, &ContextMenu::changeRenderMode);

  QObject::connect(stereomodegroup, &QActionGroup::triggered,
                   this, &ContextMenu::changeStereoMode);

  QObject::connect(transparencytypegroup, &QActionGroup::triggered,
                   this, &ContextMenu::changeTransparencyType);
}

ContextMenu::~ContextMenu()
{
  delete this->functionsmenu;
  delete this->rendermenu;
  delete this->stereomenu;
  delete this->transparencymenu;
  delete this->contextmenu;
}

QMenu *
ContextMenu::getMenu() const
{
  return this->contextmenu;
}

void
ContextMenu::changeRenderMode(QAction * action)
{
  QuarterWidget::RenderMode mode =
    static_cast<QuarterWidget::RenderMode>(action->data().toInt());

  this->quarterwidget->setRenderMode(mode);
  this->quarterwidget->getSoRenderManager()->scheduleRedraw();
}

void
ContextMenu::changeStereoMode(QAction * action)
{
  QuarterWidget::StereoMode mode =
    static_cast<QuarterWidget::StereoMode>(action->data().toInt());

  this->quarterwidget->setStereoMode(mode);
  this->quarterwidget->getSoRenderManager()->scheduleRedraw();
}

void
ContextMenu::changeTransparencyType(QAction * action)
{
  QuarterWidget::TransparencyType type =
    static_cast<QuarterWidget::TransparencyType>(action->data().toInt());

  this->quarterwidget->setTransparencyType(type);
  this->quarterwidget->getSoRenderManager()->scheduleRedraw();
}
