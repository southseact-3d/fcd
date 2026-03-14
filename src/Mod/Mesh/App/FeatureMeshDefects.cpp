#include "Core/Degeneration.h"
#include "Core/Triangulation.h"
#include <Base/Tools.h>

#include "FeatureMeshDefects.h"


using namespace Mesh;


//===========================================================================
// Defects Feature
//===========================================================================

PROPERTY_SOURCE(Mesh::FixDefects, Mesh::Feature)

FixDefects::FixDefects()
{
    ADD_PROPERTY(Source, (nullptr));
    ADD_PROPERTY(Epsilon, (0));
}

short FixDefects::mustExecute() const
{
    if (Source.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* FixDefects::execute()
{
    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::HarmonizeNormals, Mesh::FixDefects)

HarmonizeNormals::HarmonizeNormals() = default;

App::DocumentObjectExecReturn* HarmonizeNormals::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->harmonizeNormals();
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FlipNormals, Mesh::FixDefects)

FlipNormals::FlipNormals() = default;

App::DocumentObjectExecReturn* FlipNormals::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->flipNormals();
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixNonManifolds, Mesh::FixDefects)

FixNonManifolds::FixNonManifolds() = default;

App::DocumentObjectExecReturn* FixNonManifolds::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->removeNonManifolds();
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDuplicatedFaces, Mesh::FixDefects)

FixDuplicatedFaces::FixDuplicatedFaces() = default;

App::DocumentObjectExecReturn* FixDuplicatedFaces::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->removeDuplicatedFacets();
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDuplicatedPoints, Mesh::FixDefects)

FixDuplicatedPoints::FixDuplicatedPoints() = default;

App::DocumentObjectExecReturn* FixDuplicatedPoints::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->removeDuplicatedPoints();
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDegenerations, Mesh::FixDefects)

FixDegenerations::FixDegenerations() = default;

App::DocumentObjectExecReturn* FixDegenerations::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->validateDegenerations(static_cast<float>(Epsilon.getValue()));
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixDeformations, Mesh::FixDefects)

FixDeformations::FixDeformations()
{
    ADD_PROPERTY(MaxAngle, (5.0F));
}

App::DocumentObjectExecReturn* FixDeformations::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        float maxAngle = Base::toRadians(MaxAngle.getValue());
        mesh->validateDeformations(maxAngle, static_cast<float>(Epsilon.getValue()));
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FixIndices, Mesh::FixDefects)

FixIndices::FixIndices() = default;

App::DocumentObjectExecReturn* FixIndices::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->validateIndices();
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::FillHoles, Mesh::FixDefects)

FillHoles::FillHoles()
{
    ADD_PROPERTY(FillupHolesOfLength, (0));
    ADD_PROPERTY(MaxArea, (0.1F));
}

App::DocumentObjectExecReturn* FillHoles::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        MeshCore::ConstraintDelaunayTriangulator cTria((float)MaxArea.getValue());
        // MeshCore::Triangulator cTria(mesh->getKernel());
        mesh->fillupHoles(FillupHolesOfLength.getValue(), 1, cTria);
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}

// ----------------------------------------------------------------------

PROPERTY_SOURCE(Mesh::RemoveComponents, Mesh::FixDefects)

RemoveComponents::RemoveComponents()
{
    ADD_PROPERTY(RemoveCompOfSize, (0));
}

App::DocumentObjectExecReturn* RemoveComponents::execute()
{
    App::DocumentObject* link = Source.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No mesh linked");
    }
    App::Property* prop = link->getPropertyByName("Mesh");
    if (prop && prop->is<Mesh::PropertyMeshKernel>()) {
        Mesh::PropertyMeshKernel* kernel = static_cast<Mesh::PropertyMeshKernel*>(prop);
        std::unique_ptr<MeshObject> mesh(new MeshObject);
        *mesh = kernel->getValue();
        mesh->removeComponents(RemoveCompOfSize.getValue());
        this->Mesh.setValuePtr(mesh.release());
    }

    return App::DocumentObject::StdReturn;
}
