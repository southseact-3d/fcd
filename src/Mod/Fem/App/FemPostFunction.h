#ifndef Fem_FemPostFunction_H
#define Fem_FemPostFunction_H

#include <vtkBoundingBox.h>
#include <vtkBox.h>
#include <vtkCylinder.h>
#include <vtkImplicitFunction.h>
#include <vtkPlane.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>

#include <App/PropertyUnits.h>
#include <App/DocumentObjectGroup.h>

#include "FemPostObject.h"


namespace Fem
{

class FemExport FemPostFunction: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostFunction);

public:
    /// Constructor
    FemPostFunction();
    ~FemPostFunction() override;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostFunction";
    }

    App::DocumentObjectExecReturn* execute() override;

    // bound box handling
    void setBoundingBox(vtkBoundingBox b)
    {
        m_boundingBox = b;
    };

    // get the algorithm or the data
    vtkSmartPointer<vtkImplicitFunction> getImplicitFunction()
    {
        return m_implicit;
    };

protected:
    vtkSmartPointer<vtkImplicitFunction> m_implicit;
    vtkBoundingBox m_boundingBox;
};

class FemExport FemPostFunctionProvider: public App::DocumentObjectGroup
{

    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostFunctionProvider);

public:
    FemPostFunctionProvider();
    ~FemPostFunctionProvider() override;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostFunctionProvider";
    }

protected:
    bool allowObject(App::DocumentObject* obj) override;
    void unsetupObject() override;

    // update documents
    void handleChangedPropertyName(
        Base::XMLReader& reader,
        const char* TypeName,
        const char* PropName
    ) override;
};

// ---------------------------------------------------------------------------

class FemExport FemPostBoxFunction: public FemPostFunction
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostBoxFunction);

public:
    FemPostBoxFunction();
    ~FemPostBoxFunction() override;

    App::PropertyVectorDistance Center;
    App::PropertyDistance Length;
    App::PropertyDistance Width;
    App::PropertyDistance Height;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostBoxFunction";
    }

protected:
    void onChanged(const App::Property* prop) override;
    /// get called after a document has been fully restored
    void onDocumentRestored() override;

    vtkSmartPointer<vtkBox> m_box;
};

// ---------------------------------------------------------------------------

class FemExport FemPostCylinderFunction: public FemPostFunction
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostCylinderFunction);

public:
    FemPostCylinderFunction();
    ~FemPostCylinderFunction() override;

    App::PropertyVector Axis;
    App::PropertyVectorDistance Center;
    App::PropertyDistance Radius;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostCylinderFunction";
    }

protected:
    void onChanged(const App::Property* prop) override;
    /// get called after a document has been fully restored
    void onDocumentRestored() override;

    vtkSmartPointer<vtkCylinder> m_cylinder;
};

// ---------------------------------------------------------------------------

class FemExport FemPostPlaneFunction: public FemPostFunction
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostPlaneFunction);

public:
    FemPostPlaneFunction();
    ~FemPostPlaneFunction() override;

    App::PropertyVector Normal;
    App::PropertyVectorDistance Origin;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostPlaneFunction";
    }

protected:
    void onChanged(const App::Property* prop) override;
    /// get called after a document has been fully restored
    void onDocumentRestored() override;

    vtkSmartPointer<vtkPlane> m_plane;
};

// ---------------------------------------------------------------------------

class FemExport FemPostSphereFunction: public FemPostFunction
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemPostSphereFunction);

public:
    FemPostSphereFunction();
    ~FemPostSphereFunction() override;

    App::PropertyDistance Radius;
    App::PropertyVectorDistance Center;

    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemPostSphereFunction";
    }

protected:
    void onChanged(const App::Property* prop) override;

    vtkSmartPointer<vtkSphere> m_sphere;
};

}  // namespace Fem


#endif  // Fem_FemPostFunction_H
