#include <boost/uuid/uuid_io.hpp>


#include <Base/Console.h>

#include "ExternalGeometryFacade.h"
#include "ExternalGeometryFacadePy.h"

FC_LOG_LEVEL_INIT("Sketch", true, true);

using namespace Sketcher;

TYPESYSTEM_SOURCE(Sketcher::ExternalGeometryFacade, Base::BaseClass)

ExternalGeometryFacade::ExternalGeometryFacade()
    : Geo(nullptr)
    , SketchGeoExtension(nullptr)
    , ExternalGeoExtension(nullptr)
{}

ExternalGeometryFacade::ExternalGeometryFacade(const Part::Geometry* geometry)
    : Geo(geometry)
{
    if (geometry) {
        initExtensions();
    }
    else {
        THROWM(Base::ValueError, "ExternalGeometryFacade initialized with Geometry null pointer");
    }
}

std::unique_ptr<ExternalGeometryFacade> ExternalGeometryFacade::getFacade(Part::Geometry* geometry)
{
    if (geometry) {
        return std::unique_ptr<ExternalGeometryFacade>(new ExternalGeometryFacade(geometry));
    }
    else {
        return std::unique_ptr<ExternalGeometryFacade>(nullptr);
    }
}

std::unique_ptr<const ExternalGeometryFacade> ExternalGeometryFacade::getFacade(
    const Part::Geometry* geometry
)
{
    if (geometry) {
        return std::unique_ptr<const ExternalGeometryFacade>(new ExternalGeometryFacade(geometry));
    }
    else {
        return std::unique_ptr<const ExternalGeometryFacade>(nullptr);
    }
}

void ExternalGeometryFacade::setGeometry(Part::Geometry* geometry)
{
    Geo = geometry;

    if (geometry) {
        initExtensions();
    }
    else {
        THROWM(Base::ValueError, "ExternalGeometryFacade initialized with Geometry null pointer");
    }
}

void ExternalGeometryFacade::initExtensions()
{
    if (!Geo->hasExtension(SketchGeometryExtension::getClassTypeId())) {

        getGeo()->setExtension(std::make_unique<SketchGeometryExtension>());  // Create getExtension

        // The following warning was commented out as part of the Toponaming problem mitigation, and
        // left for potential usefulness to future developers making a custom build for debugging.
        // Base::Console().warning("Sketcher External Geometry without Geometry Extension: %s
        // \n",
        //                         boost::uuids::to_string(Geo->getTag()).c_str());
    }

    if (!Geo->hasExtension(ExternalGeometryExtension::getClassTypeId())) {

        getGeo()->setExtension(std::make_unique<ExternalGeometryExtension>());  // Create getExtension

        // The following warning was commented out as part of the Toponaming problem mitigation, and
        // left for potential usefulness to future developers making a custom build for debugging.
        // Base::Console().warning(
        //     "Sketcher External Geometry without ExternalGeometryExtension: %s \n",
        //     boost::uuids::to_string(Geo->getTag()).c_str());
    }

    SketchGeoExtension = std::static_pointer_cast<const SketchGeometryExtension>(
        (Geo->getExtension(SketchGeometryExtension::getClassTypeId())).lock()
    );

    ExternalGeoExtension = std::static_pointer_cast<const ExternalGeometryExtension>(
        (Geo->getExtension(ExternalGeometryExtension::getClassTypeId())).lock()
    );
}

void ExternalGeometryFacade::initExtensions() const
{
    if (!Geo->hasExtension(SketchGeometryExtension::getClassTypeId())) {
        THROWM(
            Base::ValueError,
            "ExternalGeometryFacade for const::Geometry without SketchGeometryExtension"
        );
    }

    if (!Geo->hasExtension(ExternalGeometryExtension::getClassTypeId())) {
        THROWM(
            Base::ValueError,
            "ExternalGeometryFacade for const::Geometry without ExternalGeometryExtension"
        );
    }

    auto ext = std::static_pointer_cast<const SketchGeometryExtension>(
        Geo->getExtension(SketchGeometryExtension::getClassTypeId()).lock()
    );

    const_cast<ExternalGeometryFacade*>(this)->SketchGeoExtension = ext;

    auto extext = std::static_pointer_cast<const ExternalGeometryExtension>(
        Geo->getExtension(ExternalGeometryExtension::getClassTypeId()).lock()
    );

    const_cast<ExternalGeometryFacade*>(this)->ExternalGeoExtension = extext;
}

void ExternalGeometryFacade::ensureSketchGeometryExtensions(Part::Geometry* geometry)
{
    if (!geometry->hasExtension(SketchGeometryExtension::getClassTypeId())) {
        // Create geoExtension
        geometry->setExtension(std::make_unique<SketchGeometryExtension>());
    }

    if (!geometry->hasExtension(ExternalGeometryExtension::getClassTypeId())) {
        // Create external geoExtension
        geometry->setExtension(std::make_unique<ExternalGeometryExtension>());
    }
}

void ExternalGeometryFacade::copyId(const Part::Geometry* src, Part::Geometry* dst)
{
    auto gfsrc = ExternalGeometryFacade::getFacade(src);
    auto gfdst = ExternalGeometryFacade::getFacade(dst);
    gfdst->setId(gfsrc->getId());
}

void ExternalGeometryFacade::copyFlags(const Part::Geometry* src, Part::Geometry* dst)
{
    auto gfsrc = ExternalGeometryFacade::getFacade(src);
    auto gfdst = ExternalGeometryFacade::getFacade(dst);
    gfdst->setFlags(gfsrc->getFlags());
}

void ExternalGeometryFacade::setRef(const std::string& ref)
{
    if (ref.empty() && getId() < 0) {
        FC_ERR("Cannot set reference on root geometries");
    }
    else {
        getExternalGeoExt()->setRef(ref);
    }
}

PyObject* ExternalGeometryFacade::getPyObject()
{
    return new ExternalGeometryFacadePy(new ExternalGeometryFacade(this->Geo));
}
