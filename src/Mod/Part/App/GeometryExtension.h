#ifndef PART_GEOMETRYEXTENSION_H
#define PART_GEOMETRYEXTENSION_H

#include <memory>
#include <string>

#include <Base/Persistence.h>
#include <Mod/Part/PartGlobal.h>


namespace Part
{

class Geometry;

class PartExport GeometryExtension: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    ~GeometryExtension() override = default;

    virtual std::unique_ptr<GeometryExtension> copy() const = 0;

    PyObject* getPyObject() override = 0;
    PyObject* copyPyObject() const;

    inline void setName(const std::string& str)
    {
        name = str;
    }
    inline const std::string& getName() const
    {
        return name;
    }

    // Default method to notify an extension that it has been attached
    // to a given geometry
    virtual void notifyAttachment(Part::Geometry*)
    {}

protected:
    GeometryExtension();
    GeometryExtension(const GeometryExtension& obj) = default;
    GeometryExtension& operator=(const GeometryExtension& obj) = default;

    virtual void copyAttributes(Part::GeometryExtension* cpy) const;

private:
    std::string name;
};


class PartExport GeometryPersistenceExtension: public Part::GeometryExtension
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    ~GeometryPersistenceExtension() override = default;

    // Own Persistence implementer - Not Base::Persistence - managed by Part::Geometry
    void Save(Base::Writer& /*writer*/) const;
    void Restore(Base::XMLReader& /*reader*/);

    bool isSame(const GeometryPersistenceExtension& other) const;

protected:
    virtual void restoreAttributes(Base::XMLReader& /*reader*/);
    virtual void saveAttributes(Base::Writer& writer) const;
};

}  // namespace Part

#endif  // PART_GEOMETRYEXTENSION_H
