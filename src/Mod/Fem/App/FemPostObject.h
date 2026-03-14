#ifndef Fem_FemPostObject_H
#define Fem_FemPostObject_H

#include "PropertyPostDataObject.h"
#include <App/GeoFeature.h>
#include <App/PropertyStandard.h>

#include <vtkBoundingBox.h>
#include <vtkTransformFilter.h>
#include <vtkSmartPointer.h>

class vtkDataSet;

namespace Fem
{

// poly data is the only data we can visualize, hence every post
// processing object needs to expose it
class FemExport FemPostObject: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostObject);

public:
    /// Constructor
    FemPostObject();
    ~FemPostObject() override;

    Fem::PropertyPostDataObject Data;

    // returns the DataSet from the data property. Better use this
    // instead of casting Data.getValue(), as data does not need to be a dataset,
    // but could for example also be a composite data structure.
    // Could return NULL if no dataset is available
    virtual vtkDataSet* getDataSet();

    PyObject* getPyObject() override;

    vtkBoundingBox getBoundingBox();
    void writeVTK(const char* filename) const;

protected:
    // placement is applied via transform filter. However, we do not know
    // how this filter should be used to create data. This is to be implemented
    // by the derived classes.
    vtkSmartPointer<vtkTransformFilter> m_transform_filter;

    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // Fem_FemPostObject_H
