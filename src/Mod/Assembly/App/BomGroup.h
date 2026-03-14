#ifndef ASSEMBLY_BomGroup_H
#define ASSEMBLY_BomGroup_H

#include <Mod/Assembly/AssemblyGlobal.h>

#include <App/DocumentObjectGroup.h>
#include <App/PropertyLinks.h>


namespace Assembly
{

class AssemblyExport BomGroup: public App::DocumentObjectGroup
{
    PROPERTY_HEADER_WITH_OVERRIDE(Assembly::BomGroup);

public:
    BomGroup();
    ~BomGroup() override;

    PyObject* getPyObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "AssemblyGui::ViewProviderBomGroup";
    }
};


}  // namespace Assembly


#endif  // ASSEMBLY_BomGroup_H
