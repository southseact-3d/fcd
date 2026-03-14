#include <QtUiPlugin>
#include <qplugin.h>

class QDesignerFormEditorInterface;

class CustomWidgetPlugin: public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")

public:
    CustomWidgetPlugin(QObject* parent = 0);
    QList<QDesignerCustomWidgetInterface*> customWidgets() const;
};
