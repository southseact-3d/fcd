#ifndef PATH_FeatureCompound_H
#define PATH_FeatureCompound_H

#include <App/PropertyLinks.h>

#include "FeaturePath.h"


namespace Path
{

class PathExport FeatureCompound: public Path::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Path::Feature);

public:
    /// Constructor
    FeatureCompound();
    ~FeatureCompound() override;

    App::PropertyLinkList Group;
    App::PropertyBool UsePlacements;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PathGui::ViewProviderPathCompound";
    }
    App::DocumentObjectExecReturn* execute() override;

    /// Checks whether the object \a obj is part of this group.
    bool hasObject(const DocumentObject* obj) const;
    /// Adds an object to this group.
    void addObject(DocumentObject* obj);
    /// Removes an object from this group.
    void removeObject(DocumentObject* obj);
    PyObject* getPyObject() override;
};

using FeatureCompoundPython = App::FeaturePythonT<FeatureCompound>;

}  // namespace Path


#endif  // PATH_FeatureCompound_H
