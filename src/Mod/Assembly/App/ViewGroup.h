#ifndef ASSEMBLY_ViewGroup_H
#define ASSEMBLY_ViewGroup_H

#include <Mod/Assembly/AssemblyGlobal.h>

#include <App/DocumentObjectGroup.h>
#include <App/PropertyLinks.h>


namespace Assembly
{

class AssemblyExport ViewGroup: public App::DocumentObjectGroup
{
    PROPERTY_HEADER_WITH_OVERRIDE(Assembly::ViewGroup);

public:
    ViewGroup();
    ~ViewGroup() override;

    PyObject* getPyObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "AssemblyGui::ViewProviderViewGroup";
    }
};


}  // namespace Assembly


#endif  // ASSEMBLY_ViewGroup_H
