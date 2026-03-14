#ifndef ASSEMBLY_SimulationGroup_H
#define ASSEMBLY_SimulationGroup_H

#include <Mod/Assembly/AssemblyGlobal.h>

#include <App/DocumentObjectGroup.h>
#include <App/PropertyLinks.h>


namespace Assembly
{

class AssemblyExport SimulationGroup: public App::DocumentObjectGroup
{
    PROPERTY_HEADER_WITH_OVERRIDE(Assembly::SimulationGroup);

public:
    SimulationGroup();
    ~SimulationGroup() override;

    PyObject* getPyObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "AssemblyGui::ViewProviderSimulationGroup";
    }
};


}  // namespace Assembly


#endif  // ASSEMBLY_SimulationGroup_H
