#ifndef FEATURE_MESH_SOLID_H
#define FEATURE_MESH_SOLID_H

#include "MeshFeature.h"

#include <App/PropertyUnits.h>


namespace Mesh
{

/**
 * @author Werner Mayer
 */
class Sphere: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Sphere);

public:
    Sphere();

    App::PropertyLength Radius;
    App::PropertyIntegerConstraint Sampling;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    //@}
};

// -------------------------------------------------------------

class Ellipsoid: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Ellipsoid);

public:
    Ellipsoid();

    App::PropertyLength Radius1;
    App::PropertyLength Radius2;
    App::PropertyIntegerConstraint Sampling;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    //@}
};

// -------------------------------------------------------------

class Cylinder: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Cylinder);

public:
    Cylinder();

    App::PropertyLength Radius;
    App::PropertyLength Length;
    App::PropertyLength EdgeLength;
    App::PropertyBool Closed;
    App::PropertyIntegerConstraint Sampling;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    //@}
};

// -------------------------------------------------------------

class Cone: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Cone);

public:
    Cone();

    App::PropertyLength Radius1;
    App::PropertyLength Radius2;
    App::PropertyLength Length;
    App::PropertyLength EdgeLength;
    App::PropertyBool Closed;
    App::PropertyIntegerConstraint Sampling;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    //@}
};

// -------------------------------------------------------------

class Torus: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Torus);

public:
    Torus();

    App::PropertyLength Radius1;
    App::PropertyLength Radius2;
    App::PropertyIntegerConstraint Sampling;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    //@}
};

// -------------------------------------------------------------

class Cube: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Cube);

public:
    Cube();

    App::PropertyLength Length;
    App::PropertyLength Width;
    App::PropertyLength Height;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    //@}
};

}  // namespace Mesh

#endif  // FEATURE_MESH_SOLID_H
