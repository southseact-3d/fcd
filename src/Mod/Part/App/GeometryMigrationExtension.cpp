#include <Base/Exception.h>

#include "GeometryMigrationExtension.h"


using namespace Part;

TYPESYSTEM_SOURCE_ABSTRACT(Part::GeometryMigrationPersistenceExtension, Part::GeometryPersistenceExtension)

//---------- Geometry Extension
TYPESYSTEM_SOURCE(Part::GeometryMigrationExtension, Part::GeometryExtension)


void GeometryMigrationExtension::copyAttributes(Part::GeometryExtension* cpy) const
{
    Part::GeometryExtension::copyAttributes(cpy);
    static_cast<GeometryMigrationExtension*>(cpy)->ConstructionState = this->ConstructionState;
    static_cast<GeometryMigrationExtension*>(cpy)->GeometryMigrationFlags = this->GeometryMigrationFlags;

    static_cast<GeometryMigrationExtension*>(cpy)->Id = this->Id;
    static_cast<GeometryMigrationExtension*>(cpy)->Flags = this->Flags;
    static_cast<GeometryMigrationExtension*>(cpy)->Ref = this->Ref;
    static_cast<GeometryMigrationExtension*>(cpy)->RefIndex = this->RefIndex;
}

std::unique_ptr<Part::GeometryExtension> GeometryMigrationExtension::copy() const
{
    auto cpy = std::make_unique<GeometryMigrationExtension>();

    copyAttributes(cpy.get());

    return cpy;
}

PyObject* GeometryMigrationExtension::getPyObject()
{
    THROWM(Base::NotImplementedError, "GeometryMigrationExtension does not have a Python counterpart");
}
