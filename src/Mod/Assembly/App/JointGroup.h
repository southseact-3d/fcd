#ifndef ASSEMBLY_JointGroup_H
#define ASSEMBLY_JointGroup_H

#include <Mod/Assembly/AssemblyGlobal.h>

#include <App/DocumentObjectGroup.h>
#include <App/PropertyLinks.h>


namespace Assembly
{

class AssemblyExport JointGroup: public App::DocumentObjectGroup
{
    PROPERTY_HEADER_WITH_OVERRIDE(Assembly::JointGroup);

public:
    JointGroup();
    ~JointGroup() override;

    PyObject* getPyObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "AssemblyGui::ViewProviderJointGroup";
    }

    std::vector<App::DocumentObject*> getJoints();
};


}  // namespace Assembly


#endif  // ASSEMBLY_JointGroup_H
