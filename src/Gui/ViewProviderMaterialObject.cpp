#include <QApplication>
#include <QStyle>


#include "ViewProviderMaterialObject.h"
#include "Application.h"
#include "Document.h"


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderMaterialObject, Gui::ViewProviderDocumentObject)


/**
 * Creates the view provider for an object group.
 */
ViewProviderMaterialObject::ViewProviderMaterialObject() = default;

ViewProviderMaterialObject::~ViewProviderMaterialObject() = default;

bool ViewProviderMaterialObject::doubleClicked()
{
    Gui::Application::Instance->activeDocument()->setEdit(this, (int)ViewProvider::Default);
    return true;
}

/**
 * Returns the pixmap for the list item.
 */
QIcon ViewProviderMaterialObject::getIcon() const
{
    QIcon groupIcon;
    groupIcon.addPixmap(
        QApplication::style()->standardPixmap(QStyle::SP_DirClosedIcon),
        QIcon::Normal,
        QIcon::Off
    );
    groupIcon.addPixmap(
        QApplication::style()->standardPixmap(QStyle::SP_DirOpenIcon),
        QIcon::Normal,
        QIcon::On
    );
    return groupIcon;
}


// Python feature -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderMaterialObjectPython, Gui::ViewProviderMaterialObject)
/// @endcond

// explicit template instantiation
template class GuiExport ViewProviderFeaturePythonT<ViewProviderMaterialObject>;
}  // namespace Gui
