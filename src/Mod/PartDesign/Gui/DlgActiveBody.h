#ifndef PARTDESIGNGUI_DLGACTIVEBODY_H
#define PARTDESIGNGUI_DLGACTIVEBODY_H

#include <QDialog>
#include <memory>
#include <App/Document.h>
#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/PartDesignGlobal.h>


namespace PartDesignGui
{
class Ui_DlgActiveBody;

/** Dialog box to ask user to pick a Part Design body to make active
 *  or make a new one
 */
class PartDesignGuiExport DlgActiveBody: public QDialog
{
    Q_OBJECT

public:
    DlgActiveBody(QWidget* parent, App::Document*& doc, const QString& infoText = QString());
    ~DlgActiveBody() override;

    void accept() override;
    PartDesign::Body* getActiveBody() const
    {
        return activeBody;
    }

private:
    std::unique_ptr<Ui_DlgActiveBody> ui;
    App::Document* _doc;
    PartDesign::Body* activeBody;
};

}  // namespace PartDesignGui

#endif  // PARTDESIGNGUI_DLGACTIVEBODY_H
