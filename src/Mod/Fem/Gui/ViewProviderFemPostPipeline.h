#ifndef FEM_VIEWPROVIDERFEMPOSTPIPELINE_H
#define FEM_VIEWPROVIDERFEMPOSTPIPELINE_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Fem/FemGlobal.h>

#include "Gui/ViewProviderGroupExtension.h"
#include "ViewProviderFemPostObject.h"


namespace FemGui
{

class FemGuiExport ViewProviderFemPostPipeline: public ViewProviderFemPostObject,
                                                public Gui::ViewProviderGroupExtension
{

    PROPERTY_HEADER_WITH_EXTENSIONS(FemGui::ViewProviderFemPostPipeline);

public:
    /// constructor.
    ViewProviderFemPostPipeline();
    ~ViewProviderFemPostPipeline() override;

    void updateData(const App::Property* prop) override;
    bool onDelete(const std::vector<std::string>& objs) override;
    void beforeDelete() override;
    void onSelectionChanged(const Gui::SelectionChanges& sel) override;
    void updateColorBars();
    void transformField(char* FieldName, double FieldFactor);
    void scaleField(vtkDataSet* dset, vtkDataArray* pdata, double FieldFactor);
    PyObject* getPyObject() override;

protected:
    void updateFunctionSize();
    virtual void setupTaskDialog(TaskDlgPost* dlg) override;

    // change default group drag/drop behaviour slightly
    bool acceptReorderingObjects() const override;
    bool canDragObjectToTarget(App::DocumentObject* obj, App::DocumentObject* target) const override;

    // override, to not show/hide children as the parent is shown/hidden like normal groups
    void extensionHide() override {};
    void extensionShow() override {};
};

}  // namespace FemGui


#endif  // FEM_VIEWPROVIDERFEMPOSTPIPELINE_H
