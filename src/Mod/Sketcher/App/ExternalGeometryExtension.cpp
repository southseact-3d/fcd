#include <Base/Reader.h>
#include <Base/Writer.h>

#include "ExternalGeometryExtension.h"
#include "ExternalGeometryExtensionPy.h"


using namespace Sketcher;

//---------- Geometry Extension

TYPESYSTEM_SOURCE(Sketcher::ExternalGeometryExtension, Part::GeometryMigrationPersistenceExtension)

void ExternalGeometryExtension::copyAttributes(Part::GeometryExtension* cpy) const
{
    Part::GeometryPersistenceExtension::copyAttributes(cpy);

    static_cast<ExternalGeometryExtension*>(cpy)->Ref = this->Ref;
    static_cast<ExternalGeometryExtension*>(cpy)->RefIndex = this->RefIndex;
    static_cast<ExternalGeometryExtension*>(cpy)->Flags = this->Flags;
}

void ExternalGeometryExtension::restoreAttributes(Base::XMLReader& reader)
{
    Part::GeometryPersistenceExtension::restoreAttributes(reader);

    Ref = reader.getAttribute<const char*>("Ref", "");
    RefIndex = reader.getAttribute<long>("RefIndex", -1);
    Flags = FlagType(reader.getAttribute<unsigned long>("Flags", 0));
}

void ExternalGeometryExtension::saveAttributes(Base::Writer& writer) const
{
    Part::GeometryPersistenceExtension::saveAttributes(writer);
    writer.Stream() << "\" Ref=\"" << Base::Persistence::encodeAttribute(Ref);
    writer.Stream() << "\" Flags=\"" << Flags.to_ulong();
    if (RefIndex >= 0) {
        writer.Stream() << "\" RefIndex=\"" << RefIndex;
    }
}

void ExternalGeometryExtension::preSave(Base::Writer& writer) const
{
    if (Ref.size()) {
        writer.Stream() << " ref=\"" << Base::Persistence::encodeAttribute(Ref) << "\"";
    }
    if (RefIndex >= 0) {
        writer.Stream() << " refIndex=\"" << RefIndex << "\"";
    }
    if (Flags.any()) {
        writer.Stream() << " flags=\"" << Flags.to_ulong() << "\"";
    }
}

std::unique_ptr<Part::GeometryExtension> ExternalGeometryExtension::copy() const
{
    auto cpy = std::make_unique<ExternalGeometryExtension>();

    copyAttributes(cpy.get());

    return cpy;
}

PyObject* ExternalGeometryExtension::getPyObject()
{
    return new ExternalGeometryExtensionPy(new ExternalGeometryExtension(*this));
}

bool ExternalGeometryExtension::getFlagsFromName(std::string str, ExternalGeometryExtension::Flag& flag)
{
    auto pos = std::find_if(
        ExternalGeometryExtension::flag2str.begin(),
        ExternalGeometryExtension::flag2str.end(),
        [str](const char* val) { return strcmp(val, str.c_str()) == 0; }
    );

    if (pos != ExternalGeometryExtension::flag2str.end()) {
        int index = std::distance(ExternalGeometryExtension::flag2str.begin(), pos);

        flag = static_cast<ExternalGeometryExtension::Flag>(index);
        return true;
    }

    return false;
}
