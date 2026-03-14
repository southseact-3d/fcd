#ifndef PARTGUI_TASKSETCOLORS_H
#define PARTGUI_TASKSETCOLORS_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

namespace Gui
{
class Document;
class ViewProvider;
}  // namespace Gui

namespace Materials
{
class Material;
}

namespace PartGui
{

class ViewProviderPartExt;

class FaceAppearances: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit FaceAppearances(ViewProviderPartExt* vp, QWidget* parent = nullptr);
    ~FaceAppearances() override;

    void open();
    bool accept();
    bool reject();

private:
    void setupConnections();
    void onMaterialSelected(const std::shared_ptr<Materials::Material>& material);
    void onDefaultButtonClicked();
    void onBoxSelectionToggled(bool checked);
    void onButtonCustomAppearanceClicked();

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void changeEvent(QEvent* e) override;
    void slotUndoDocument(const Gui::Document& Doc);
    void slotDeleteDocument(const Gui::Document&);
    void slotDeleteObject(const Gui::ViewProvider&);
    void updatePanel();
    int getFirstIndex() const;

private:
    class Private;
    Private* d;
};

class TaskFaceAppearances: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskFaceAppearances(ViewProviderPartExt* vp);
    ~TaskFaceAppearances() override;

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    FaceAppearances* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKSETCOLORS_H
