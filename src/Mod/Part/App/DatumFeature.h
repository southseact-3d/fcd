#ifndef PART_DATUMFEATURE_H
#define PART_DATUMFEATURE_H

#include <Mod/Part/PartGlobal.h>

#include "AttachExtension.h"


namespace Part
{

// This generic class is defined here so that the Sketcher module can access datum features
// without creating a dependency on PartDesign

class PartExport Datum: public Part::Feature, public Part::AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Part::Datum);

public:
    Datum();
    ~Datum() override;
    // short mustExecute();

    /// returns the type name of the view provider
    const char* getViewProviderName() const override = 0;

    /// Return a shape including Placement representing the datum feature
    virtual TopoDS_Shape getShape() const;

    /// Returns a point of the feature it counts as it's base
    virtual Base::Vector3d getBasePoint() const;

    App::DocumentObject* getSubObject(
        const char* subname,
        PyObject** pyObj,
        Base::Matrix4D* mat,
        bool transform,
        int depth
    ) const override;

protected:
    void onDocumentRestored() override;
};

}  // namespace Part


#endif  // PART_DATUMFEATURE_H
