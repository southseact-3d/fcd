#include <Gui/BitmapFactory.h>
#include <Gui/Control.h>

#include "TaskDlgPathCompound.h"


using namespace Gui;
using namespace PathGui;

PROPERTY_SOURCE(PathGui::ViewProviderPathCompound, PathGui::ViewProviderPath)


bool ViewProviderPathCompound::setEdit(int ModNum)
{
    Q_UNUSED(ModNum);
    Gui::TaskView::TaskDialog* dlg = new TaskDlgPathCompound(this);
    Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderPathCompound::unsetEdit(int ModNum)
{
    Q_UNUSED(ModNum);
    // when pressing ESC make sure to close the dialog
    Gui::Control().closeDialog();
}

std::vector<App::DocumentObject*> ViewProviderPathCompound::claimChildren() const
{
    return std::vector<App::DocumentObject*>(getObject<Path::FeatureCompound>()->Group.getValues());
}

bool ViewProviderPathCompound::canDragObjects() const
{
    return true;
}

void ViewProviderPathCompound::dragObject(App::DocumentObject* obj)
{
    getObject<Path::FeatureCompound>()->removeObject(obj);
}

bool ViewProviderPathCompound::canDropObjects() const
{
    return true;
}

void ViewProviderPathCompound::dropObject(App::DocumentObject* obj)
{
    getObject<Path::FeatureCompound>()->addObject(obj);
}

QIcon ViewProviderPathCompound::getIcon() const
{
    return Gui::BitmapFactory().pixmap("CAM_Compound");
}

// Python object -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(PathGui::ViewProviderPathCompoundPython, PathGui::ViewProviderPathCompound)
/// @endcond

// explicit template instantiation
template class PathGuiExport ViewProviderFeaturePythonT<PathGui::ViewProviderPathCompound>;
}  // namespace Gui
