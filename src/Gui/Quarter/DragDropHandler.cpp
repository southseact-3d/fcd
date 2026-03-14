#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QStringList>
#include <QUrl>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>

#include "QuarterWidget.h"
#include "eventhandlers/DragDropHandler.h"


namespace SIM { namespace Coin3D { namespace Quarter {

class DragDropHandlerP {
public:
  explicit DragDropHandlerP(DragDropHandler * master) {
    this->master = master;
  }
  void dragEnterEvent(QDragEnterEvent * event);
  void dropEvent(QDropEvent * event);

  QStringList suffixes;
  DragDropHandler * master;
  QuarterWidget * quarterwidget;
};

}}} // namespace

#define PRIVATE(obj) obj->pimpl
#define PUBLIC(obj) obj->master

using namespace SIM::Coin3D::Quarter;

/*!
  Constructor

  \sa QObject::QObject(QObject *)
*/
DragDropHandler::DragDropHandler(QuarterWidget * parent)
  : QObject(parent)
{
  PRIVATE(this) = new DragDropHandlerP(this);
  PRIVATE(this)->quarterwidget = parent;
  assert(PRIVATE(this)->quarterwidget);
  PRIVATE(this)->suffixes << "iv" << "wrl";
}

DragDropHandler::~DragDropHandler()
{
  delete PRIVATE(this);
}

/*!
  Detects a QDragEnterEvent and if the event is the dropping of a
  valid Inventor or VRML file it opens the file, reads in the scene graph
  and calls setSceneGraph on the QuarterWidget
 */
bool
DragDropHandler::eventFilter(QObject *, QEvent * event)
{
  switch (event->type()) {
  case QEvent::DragEnter:
    PRIVATE(this)->dragEnterEvent(dynamic_cast<QDragEnterEvent *>(event));
    return true;
  case QEvent::Drop:
    PRIVATE(this)->dropEvent(dynamic_cast<QDropEvent *>(event));
    return true;
  default:
    return false;
  }
}

void
DragDropHandlerP::dragEnterEvent(QDragEnterEvent * event)
{
  const QMimeData * mimedata = event->mimeData();
  if (!mimedata->hasUrls() && !mimedata->hasText())
      return;

  if (mimedata->hasUrls()) {
    QFileInfo fileinfo(mimedata->urls().constFirst().path());
    QString suffix = fileinfo.suffix().toLower();
    if (!this->suffixes.contains(suffix)) {
        return;
    }
  }

  event->acceptProposedAction();
}

void
DragDropHandlerP::dropEvent(QDropEvent * event)
{
  const QMimeData * mimedata = event->mimeData();

  SoSeparator * root;
  SoInput in;
  QByteArray bytes;

  if (mimedata->hasUrls()) {
    QUrl url = mimedata->urls().constFirst();
    if (url.scheme().isEmpty() || url.scheme().toLower() == QString("file") ) {
      // attempt to open file
      if (!in.openFile(url.toLocalFile().toLatin1().constData()))
          return;
    }
  } else if (mimedata->hasText()) {
    /* FIXME 2007-11-09 preng: dropping text buffer does not work on Windows Vista. */
    bytes = mimedata->text().toUtf8();
    in.setBuffer((void *) bytes.constData(), bytes.size());
    if (!in.isValidBuffer())
        return;
  }

  // attempt to import it
  root = SoDB::readAll(&in);
  if (!root)
      return;

  // set new scene graph
  this->quarterwidget->setSceneGraph(root);
  this->quarterwidget->viewport()->update();
}

#undef PRIVATE
#undef PUBLIC
