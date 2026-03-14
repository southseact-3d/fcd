#ifndef FEM_ViewProviderResult_H
#define FEM_ViewProviderResult_H

#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Fem/FemGlobal.h>

namespace FemGui
{

class FemGuiExport ViewProviderResult: public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderResult);

public:
    /// constructor
    ViewProviderResult();

    /// destructor
    ~ViewProviderResult() override;

    // shows solid in the tree
    bool isShow() const override
    {
        return true;
    }
};

using ViewProviderResultPython = Gui::ViewProviderFeaturePythonT<ViewProviderResult>;

}  // namespace FemGui


#endif  // FEM_ViewProviderResult_H
