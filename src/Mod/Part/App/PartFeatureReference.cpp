#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>


#include <Base/Placement.h>
#include <Base/Rotation.h>

#include "PartFeatureReference.h"


using namespace Part;

PROPERTY_SOURCE(Part::FeatureReference, App::GeoFeature)

FeatureReference::FeatureReference()
{
    ADD_PROPERTY(Reference, (nullptr));
}

FeatureReference::~FeatureReference() = default;

short FeatureReference::mustExecute() const
{
    return GeoFeature::mustExecute();
}

App::DocumentObjectExecReturn* FeatureReference::execute()
{
    return App::DocumentObject::StdReturn;
}

TopLoc_Location FeatureReference::getLocation() const
{
    Base::Placement pl = this->Placement.getValue();
    Base::Rotation rot(pl.getRotation());
    Base::Vector3d axis;
    double angle;
    rot.getValue(axis, angle);
    gp_Trsf trf;
    trf.SetRotation(gp_Ax1(gp_Pnt(), gp_Dir(axis.x, axis.y, axis.z)), angle);
    trf.SetTranslationPart(gp_Vec(pl.getPosition().x, pl.getPosition().y, pl.getPosition().z));
    return TopLoc_Location(trf);
}

// ---------------------------------------------------------
