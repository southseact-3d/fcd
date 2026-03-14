#include <cmath>

#include <Gui/Application.h>
#include <Gui/Control.h>
#include <Gui/ViewProviderDocumentObject.h>

#include "PropertyEnumAttacherItem.h"

using namespace PartGui;

PROPERTYITEM_SOURCE(PartGui::PropertyEnumAttacherItem)

PropertyEnumAttacherItem::PropertyEnumAttacherItem() = default;

QWidget* PropertyEnumAttacherItem::
    createEditor(QWidget* parent, const std::function<void()>& method, FrameOption /*frameOption*/) const
{
    Gui::LabelButton* modeEditor = new Gui::LabelButton(parent);
    QObject::connect(modeEditor, &Gui::LabelButton::valueChanged, method);
    QObject::connect(
        modeEditor,
        &Gui::LabelButton::buttonClicked,
        this,
        &PropertyEnumAttacherItem::openTask
    );
    modeEditor->setDisabled(isReadOnly());
    return modeEditor;
}

void PropertyEnumAttacherItem::setEditorData(QWidget* editor, const QVariant& data) const
{
    Gui::LabelButton* modeEditor = qobject_cast<Gui::LabelButton*>(editor);
    modeEditor->setValue(data);
}

QVariant PropertyEnumAttacherItem::editorData(QWidget* editor) const
{
    Gui::LabelButton* modeEditor = qobject_cast<Gui::LabelButton*>(editor);
    return modeEditor->value();
}

void PropertyEnumAttacherItem::openTask()
{
    Gui::TaskView::TaskDialog* dlg = Gui::Control().activeDialog();
    TaskDlgAttacher* task;
    task = qobject_cast<TaskDlgAttacher*>(dlg);

    if (dlg && !task) {
        // there is already another task dialog which must be closed first
        Gui::Control().showDialog(dlg);
        return;
    }
    if (!task) {
        const App::Property* prop = getFirstProperty();
        if (prop) {
            App::PropertyContainer* parent = prop->getContainer();

            if (parent->isDerivedFrom<App::DocumentObject>()) {
                App::DocumentObject* obj = static_cast<App::DocumentObject*>(parent);
                Gui::ViewProvider* view = Gui::Application::Instance->getViewProvider(obj);

                if (view->isDerivedFrom<Gui::ViewProviderDocumentObject>()) {
                    task = new TaskDlgAttacher(static_cast<Gui::ViewProviderDocumentObject*>(view));
                }
            }
        }
        if (!task) {
            return;
        }
    }

    Gui::Control().showDialog(task);
}

#include "moc_PropertyEnumAttacherItem.cpp"
