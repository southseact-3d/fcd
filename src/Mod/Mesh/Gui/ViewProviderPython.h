#ifndef MESHGUI_VIEWPROVIDERPYTHON_H
#define MESHGUI_VIEWPROVIDERPYTHON_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Mesh/Gui/ViewProviderMeshFaceSet.h>

namespace MeshGui
{

using ViewProviderPython = Gui::ViewProviderFeaturePythonT<ViewProviderMeshFaceSet>;

}  // namespace MeshGui


#endif  // MESHGUI_VIEWPROVIDERPYTHON_H
