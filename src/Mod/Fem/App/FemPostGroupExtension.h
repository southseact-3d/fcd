#ifndef Fem_FemPostGroup_H
#define Fem_FemPostGroup_H

#include "Base/Unit.h"
#include "App/GroupExtension.h"
#include "FemPostFilter.h"

namespace Fem
{

enum PostGroupMode
{
    Serial,
    Parallel
};

// object grouping FEM filters and building the structure of the pipeline
class FemExport FemPostGroupExtension: public App::GroupExtension
{

    using inherited = App::GroupExtension;
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostGroupExtension);

public:
    /// Constructor
    FemPostGroupExtension();
    ~FemPostGroupExtension() override;

    void initExtension(App::ExtensionContainer* obj) override;

    App::PropertyEnumeration Mode;

    // Pipeline handling
    virtual void filterChanged(FemPostFilter*) {};          // settings change in filter
    virtual void filterPipelineChanged(FemPostFilter*) {};  // pipeline change in filter
    virtual void recomputeChildren();
    virtual FemPostObject* getLastPostObject();
    virtual bool holdsPostObject(FemPostObject* obj);

    // general
    std::vector<Fem::FemPostFilter*> getFilter();
    static App::DocumentObject* getGroupOfObject(const App::DocumentObject* obj);

protected:
    void extensionOnChanged(const App::Property* p) override;
    void onExtendedUnsetupObject() override;
    bool allowObject(App::DocumentObject* obj) override;

private:
    bool m_blockChange = false;
    static const char* ModeEnums[];
};

}  // namespace Fem


#endif  // Fem_FemPostGroup_H
