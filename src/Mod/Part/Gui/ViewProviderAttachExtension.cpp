#include <QAction>
#include <QMenu>

#include <Gui/ActionFunction.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Control.h>
#include <Mod/Part/App/AttachExtension.h>

#include "ViewProviderAttachExtension.h"
#include "TaskAttacher.h"


using namespace PartGui;

EXTENSION_PROPERTY_SOURCE(PartGui::ViewProviderAttachExtension, Gui::ViewProviderExtension)


ViewProviderAttachExtension::ViewProviderAttachExtension()
{
    initExtensionType(ViewProviderAttachExtension::getExtensionClassTypeId());
}

QIcon ViewProviderAttachExtension::extensionMergeColorfullOverlayIcons(const QIcon& orig) const
{
    QIcon mergedicon = orig;

    if (getExtendedViewProvider()->getObject()->hasExtension(
            Part::AttachExtension::getExtensionClassTypeId()
        )) {

        auto* attach
            = getExtendedViewProvider()->getObject()->getExtensionByType<Part::AttachExtension>();

        if (attach) {

            if (!attach->isAttacherActive()) {
                static QPixmap px(Gui::BitmapFactory().pixmapFromSvg("Part_Detached", QSize(10, 10)));

                mergedicon = Gui::BitmapFactoryInst::mergePixmap(
                    mergedicon,
                    px,
                    Gui::BitmapFactoryInst::BottomLeft
                );
            }
        }
    }

    return mergedicon;
}

void ViewProviderAttachExtension::extensionUpdateData(const App::Property* prop)
{
    auto obj = getExtendedViewProvider()->getObject();
    if (obj && obj->hasExtension(Part::AttachExtension::getExtensionClassTypeId())) {
        auto* attach = obj->getExtensionByType<Part::AttachExtension>();

        if (attach) {
            if (prop == &(attach->AttachmentSupport) || prop == &(attach->MapMode)
                || prop == &(attach->MapPathParameter) || prop == &(attach->MapReversed)
                || prop == &(attach->AttachmentOffset) || prop == &(attach->AttacherType)) {

                getExtendedViewProvider()->signalChangeIcon();  // signal icon change
            }
        }
    }
}

void ViewProviderAttachExtension::extensionSetupContextMenu(QMenu* menu, QObject*, const char*)
{
    bool attach = getExtendedViewProvider()->getObject()->hasExtension(
        Part::AttachExtension::getExtensionClassTypeId()
    );
    if (attach) {
        // toggle command to display components
        Gui::ActionFunction* func = new Gui::ActionFunction(menu);
        QAction* act = menu->addAction(QObject::tr("Attachment Editor"));
        if (Gui::Control().activeDialog()) {
            act->setDisabled(true);
        }
        func->trigger(act, [this]() { this->showAttachmentEditor(); });
    }
}

void ViewProviderAttachExtension::showAttachmentEditor(
    std::function<void()> onAccept,
    std::function<void()> onReject
)
{
    if (Gui::Control().activeDialog()) {
        Gui::Control().closeDialog();
    }

    TaskDlgAttacher* task = new TaskDlgAttacher(getExtendedViewProvider(), true, onAccept, onReject);
    Gui::Control().showDialog(task);
}

namespace Gui
{
EXTENSION_PROPERTY_SOURCE_TEMPLATE(
    PartGui::ViewProviderAttachExtensionPython,
    PartGui::ViewProviderAttachExtension
)

// explicit template instantiation
template class PartGuiExport ViewProviderExtensionPythonT<PartGui::ViewProviderAttachExtension>;
}  // namespace Gui
