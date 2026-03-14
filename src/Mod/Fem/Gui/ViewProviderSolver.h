#ifndef FEM_ViewProviderSolver_H
#define FEM_ViewProviderSolver_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Fem/FemGlobal.h>


class SoCoordinate3;
class SoDrawStyle;
class SoIndexedFaceSet;
class SoIndexedLineSet;
class SoShapeHints;
class SoMaterialBinding;

namespace FemGui
{

class FemGuiExport ViewProviderSolver: public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderSolver);

public:
    /// constructor
    ViewProviderSolver();

    /// destructor
    ~ViewProviderSolver() override;

    // shows solid in the tree
    bool isShow() const override
    {
        return Visibility.getValue();
    }

    // handling when object is deleted
    bool onDelete(const std::vector<std::string>&) override;
    bool canDelete(App::DocumentObject* obj) const override;
};

using ViewProviderSolverPython = Gui::ViewProviderFeaturePythonT<ViewProviderSolver>;

}  // namespace FemGui


#endif  // FEM_ViewProviderSolver_H
