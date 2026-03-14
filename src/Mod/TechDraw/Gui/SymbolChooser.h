#ifndef TECHDRAWGUI_SYMBOLCHOOSER_H
#define TECHDRAWGUI_SYMBOLCHOOSER_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QDialog>

class QListWidgetItem;

namespace TechDrawGui {

class Ui_SymbolChooser;
class TechDrawGuiExport SymbolChooser : public QDialog
{
    Q_OBJECT

public:
    SymbolChooser(QWidget *parent = nullptr,
                  QString startDir = QString(),
                  QString source = QString());
    ~SymbolChooser() override;

public Q_SLOTS:
    void onOKClicked();
    void onCancelClicked();
    void onItemClicked(QListWidgetItem* item);
    void onDirectoryChanged(const QString& newDir);

Q_SIGNALS:
    void symbolSelected(QString symbolPath,
                        QString source);

protected:
    void setUiPrimary(void);
    void loadSymbolNames(QString pathToSymbols);

private:
    std::unique_ptr<Ui_SymbolChooser> ui;
    QString m_symbolDir;
    QString m_symbolPath;
    QString m_source;
};

}
#endif // #ifndef TECHDRAWGUI_SYMBOLCHOOSER_H

