#include "FemPostFunction.h"
#include <App/Document.h>


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemPostFunctionProvider, App::DocumentObject)

FemPostFunctionProvider::FemPostFunctionProvider()
    : DocumentObjectGroup()
{}

FemPostFunctionProvider::~FemPostFunctionProvider() = default;

bool FemPostFunctionProvider::allowObject(App::DocumentObject* obj)
{
    return obj->isDerivedFrom<FemPostFunction>();
}

void FemPostFunctionProvider::unsetupObject()
{
    // remove all children!
    auto document = getExtendedObject()->getDocument();
    for (const auto& obj : Group.getValues()) {
        document->removeObject(obj->getNameInDocument());
    }
}

void FemPostFunctionProvider::handleChangedPropertyName(
    Base::XMLReader& reader,
    const char* typeName,
    const char* propName
)
{
    if (strcmp(propName, "Functions") == 0
        && Base::Type::fromName(typeName) == App::PropertyLinkList::getClassTypeId()) {

        // restore the property into Group, instead of the old Functions property
        Group.Restore(reader);
    }
    else {
        App::DocumentObject::handleChangedPropertyName(reader, typeName, propName);
    }
}

PROPERTY_SOURCE(Fem::FemPostFunction, App::DocumentObject)

FemPostFunction::FemPostFunction() = default;

FemPostFunction::~FemPostFunction() = default;

DocumentObjectExecReturn* FemPostFunction::execute()
{

    return DocumentObject::StdReturn;
}

// ***************************************************************************
// box function
PROPERTY_SOURCE(Fem::FemPostBoxFunction, Fem::FemPostFunction)

FemPostBoxFunction::FemPostBoxFunction()
    : FemPostFunction()
{
    ADD_PROPERTY(Center, (Base::Vector3d(0.0, 0.0, 0.0)));
    ADD_PROPERTY(Length, (10.0));
    ADD_PROPERTY(Width, (10.0));
    ADD_PROPERTY(Height, (10.0));

    m_box = vtkSmartPointer<vtkBox>::New();
    m_implicit = m_box;

    m_box->SetBounds(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
}

FemPostBoxFunction::~FemPostBoxFunction() = default;

void FemPostBoxFunction::onChanged(const Property* prop)
{
    if (prop == &Center || prop == &Length || prop == &Width || prop == &Height) {
        const Base::Vector3d& vec = Center.getValue();
        float l = Length.getValue();
        float w = Width.getValue();
        float h = Height.getValue();
        m_box->SetBounds(
            vec[0] - l / 2,
            vec[0] + l / 2,
            vec[1] - w / 2,
            vec[1] + w / 2,
            vec[2] - h / 2,
            vec[2] + h / 2
        );
    }

    Fem::FemPostFunction::onChanged(prop);
}

void FemPostBoxFunction::onDocumentRestored()
{
    // This is to notify the view provider that the document has been fully restored
    Center.touch();
}


// ***************************************************************************
// cylinder function
PROPERTY_SOURCE(Fem::FemPostCylinderFunction, Fem::FemPostFunction)

FemPostCylinderFunction::FemPostCylinderFunction()
    : FemPostFunction()
{
    ADD_PROPERTY(Center, (Base::Vector3d(0.0, 0.0, 0.0)));
    ADD_PROPERTY(Axis, (Base::Vector3d(0.0, 0.0, 1.0)));
    ADD_PROPERTY(Radius, (5.0));

    m_cylinder = vtkSmartPointer<vtkCylinder>::New();
    m_implicit = m_cylinder;

    m_cylinder->SetAxis(0.0, 0.0, 1.0);
    m_cylinder->SetCenter(0.0, 0.0, 0.0);
    m_cylinder->SetRadius(5.0);
}

FemPostCylinderFunction::~FemPostCylinderFunction() = default;

void FemPostCylinderFunction::onChanged(const Property* prop)
{
    if (prop == &Axis) {
        const Base::Vector3d& vec = Axis.getValue();
        m_cylinder->SetAxis(vec[0], vec[1], vec[2]);
    }
    else if (prop == &Center) {
        const Base::Vector3d& vec = Center.getValue();
        m_cylinder->SetCenter(vec[0], vec[1], vec[2]);
    }
    else if (prop == &Radius) {
        m_cylinder->SetRadius(Radius.getValue());
    }

    Fem::FemPostFunction::onChanged(prop);
}

void FemPostCylinderFunction::onDocumentRestored()
{
    // This is to notify the view provider that the document has been fully restored
    Axis.touch();
}


// ***************************************************************************
// plane function
PROPERTY_SOURCE(Fem::FemPostPlaneFunction, Fem::FemPostFunction)

FemPostPlaneFunction::FemPostPlaneFunction()
    : FemPostFunction()
{

    ADD_PROPERTY(Origin, (Base::Vector3d(0.0, 0.0, 0.0)));
    ADD_PROPERTY(Normal, (Base::Vector3d(0.0, 0.0, 1.0)));

    m_plane = vtkSmartPointer<vtkPlane>::New();
    m_implicit = m_plane;

    m_plane->SetOrigin(0.0, 0.0, 0.0);
    m_plane->SetNormal(0.0, 0.0, 1.0);
}

FemPostPlaneFunction::~FemPostPlaneFunction() = default;

void FemPostPlaneFunction::onChanged(const Property* prop)
{

    if (prop == &Origin) {
        const Base::Vector3d& vec = Origin.getValue();
        m_plane->SetOrigin(vec[0], vec[1], vec[2]);
    }
    else if (prop == &Normal) {
        const Base::Vector3d& vec = Normal.getValue();
        m_plane->SetNormal(vec[0], vec[1], vec[2]);
    }

    Fem::FemPostFunction::onChanged(prop);
}

void FemPostPlaneFunction::onDocumentRestored()
{
    // This is to notify the view provider that the document has been fully restored
    Normal.touch();
}


// ***************************************************************************
// sphere function
PROPERTY_SOURCE(Fem::FemPostSphereFunction, Fem::FemPostFunction)

FemPostSphereFunction::FemPostSphereFunction()
    : FemPostFunction()
{

    ADD_PROPERTY(Radius, (5.0));
    ADD_PROPERTY(Center, (Base::Vector3d(1.0, 0.0, 0.0)));

    m_sphere = vtkSmartPointer<vtkSphere>::New();
    m_implicit = m_sphere;

    m_sphere->SetCenter(0.0, 0.0, 0.0);
    m_sphere->SetRadius(5.0);
}

FemPostSphereFunction::~FemPostSphereFunction() = default;

void FemPostSphereFunction::onChanged(const Property* prop)
{

    if (prop == &Center) {
        const Base::Vector3d& vec = Center.getValue();
        m_sphere->SetCenter(vec[0], vec[1], vec[2]);
    }
    else if (prop == &Radius) {
        m_sphere->SetRadius(Radius.getValue());
    }

    Fem::FemPostFunction::onChanged(prop);
}
