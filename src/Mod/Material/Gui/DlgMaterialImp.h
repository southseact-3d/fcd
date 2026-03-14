#ifndef MATGUI_DIALOGMATERIALIMP_H
#define MATGUI_DIALOGMATERIALIMP_H

#include <QDialog>
#include <memory>
#include <vector>

#include <App/Material.h>
#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

#include <Mod/Material/App/Materials.h>

namespace App
{
class Property;
}

namespace MatGui
{

class ViewProvider;
class Command;

/**
 * The DlgMaterialImp class implements a dialog containing all available document
 * templates to create a new document.
 * \author Jürgen Riegel
 */
class DlgMaterialImp: public QDialog, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    explicit DlgMaterialImp(bool floating,
                            QWidget* parent = nullptr,
                            Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgMaterialImp() override;
    /// Observer message from the Selection
    void OnChange(Gui::SelectionSingleton::SubjectType& rCaller,
                  Gui::SelectionSingleton::MessageType Reason) override;
    void showDefaultButtons(bool);
    void reject() override;

private Q_SLOTS:
    void onMaterialSelected(const std::shared_ptr<Materials::Material>& material);

protected:
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();
    void slotChangedObject(const Gui::ViewProvider&, const App::Property& Prop);
    void setMaterial(const std::vector<App::DocumentObject*>&);
    std::vector<Gui::ViewProvider*> getSelection() const;
    std::vector<App::DocumentObject*> getSelectionObjects() const;

private:
    class Private;
    std::unique_ptr<Private> d;
};

class TaskMaterial: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskMaterial();
    ~TaskMaterial() override;

public:
    bool reject() override;

    bool isAllowedAlterDocument() const override
    {
        return true;
    }
    bool isAllowedAlterView() const override
    {
        return true;
    }
    bool isAllowedAlterSelection() const override
    {
        return true;
    }
    QDialogButtonBox::StandardButtons getStandardButtons() const override;

private:
    DlgMaterialImp* widget;
    Gui::TaskView::TaskBox* taskbox;
};

}  // namespace MatGui

#endif  // MATGUI_DIALOGMATERIALIMP_H
