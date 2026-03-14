#ifndef Fem_FemPostBranchFilter_H
#define Fem_FemPostBranchFilter_H


#include "FemPostFilter.h"
#include "FemPostGroupExtension.h"

#include <vtkSmartPointer.h>
#include <vtkAppendFilter.h>
#include <vtkPassThrough.h>


namespace Fem
{

class FemExport FemPostBranchFilter: public Fem::FemPostFilter, public FemPostGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Fem::FemPostBranchFilter);

public:
    /// Constructor
    FemPostBranchFilter();

    App::PropertyEnumeration Output;


    short mustExecute() const override;
    PyObject* getPyObject() override;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostBranchFilter";
    }

    // Branch handling
    void filterChanged(FemPostFilter* filter) override;
    void filterPipelineChanged(FemPostFilter* filter) override;

protected:
    void onChanged(const App::Property* prop) override;

private:
    static const char* OutputEnums[];

    void setupPipeline();

    vtkSmartPointer<vtkAppendFilter> m_append;
    vtkSmartPointer<vtkPassThrough> m_passthrough;
};

}  // namespace Fem


#endif  // Fem_FemPostBranchFilter_H
