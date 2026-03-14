#include <Base/Reader.h>
#include <Base/Writer.h>

#include "GeometryExtension.h"
#include "GeometryExtensionPy.h"


using namespace Part;

TYPESYSTEM_SOURCE_ABSTRACT(Part::GeometryExtension, Base::BaseClass)

GeometryExtension::GeometryExtension() = default;

PyObject* GeometryExtension::copyPyObject() const
{
    Py::Tuple tuple;
    Py::Object obj = Py::asObject(const_cast<GeometryExtension*>(this)->getPyObject());
    return static_cast<GeometryExtensionPy*>(obj.ptr())->copy(tuple.ptr());
}

void GeometryExtension::copyAttributes(Part::GeometryExtension* cpy) const
{
    cpy->setName(this->getName());  // Base Class
}

TYPESYSTEM_SOURCE_ABSTRACT(Part::GeometryPersistenceExtension, Part::GeometryExtension)

void GeometryPersistenceExtension::restoreAttributes(Base::XMLReader& reader)
{
    if (reader.hasAttribute("name")) {
        std::string name = reader.getAttribute<const char*>("name");
        setName(name);
    }
}
void GeometryPersistenceExtension::saveAttributes(Base::Writer& writer) const
{
    const std::string name = getName();

    if (!name.empty()) {
        writer.Stream() << "\" name=\"" << name;
    }
}

void GeometryPersistenceExtension::Save(Base::Writer& writer) const
{
    writer.Stream() << writer.ind() << "<GeoExtension type=\"" << this->getTypeId().getName();

    saveAttributes(writer);

    writer.Stream() << "\"/>" << std::endl;
}

void GeometryPersistenceExtension::Restore(Base::XMLReader& reader)
{
    restoreAttributes(reader);
}

bool GeometryPersistenceExtension::isSame(const GeometryPersistenceExtension& other) const
{
    static Base::StringWriter writer, writer2;
    //    writer.clear();
    Save(writer);
    //    writer2.clear();
    other.Save(writer2);
    return writer.getString() == writer2.getString();
}
