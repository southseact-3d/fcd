#ifndef PART_GEOMETRYDEFAULTEXTENSION_H
#define PART_GEOMETRYDEFAULTEXTENSION_H

#include <string>
#include "GeometryExtension.h"

#include <Mod/Part/PartGlobal.h>


namespace Part
{

template<typename T>
class PartExport GeometryDefaultExtension: public Part::GeometryPersistenceExtension
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    inline GeometryDefaultExtension();
    explicit GeometryDefaultExtension(const T& val, std::string name = std::string());
    ~GeometryDefaultExtension() override = default;

    inline void setValue(const T& val)
    {
        value = val;
    };
    inline const T& getValue() const
    {
        return value;
    };

    std::unique_ptr<Part::GeometryExtension> copy() const override;

    PyObject* getPyObject() override;

protected:
    void copyAttributes(Part::GeometryExtension* cpy) const override;
    void restoreAttributes(Base::XMLReader& reader) override;
    void saveAttributes(Base::Writer& writer) const override;

private:
    GeometryDefaultExtension(const GeometryDefaultExtension<T>&) = default;

private:
    T value;
};

// Description:
//
// This template allows one to define a geometry extension for a given type (uniform interface for
// one value of type T).
//
// Warnings:
// - The default constructor relies on the default constructor of T for initialisation. Built-in
// types
//   so constructed will be uninitialised. Use the specific constructor from a T to initialise it.
//   Note that the default constructor is required by the type system (see
//   TYPESYSTEM_SOURCE_TEMPLATE_T).
//
// Default assumptions:
// - T can be constructed from T
// - T can be assigned to T
// - T is convertible to a std::string
// - T is serialisable as a string
//
// template specialisation:
//
// If the assumptions do not meet for your type, template specialisation allows you to provide
// specific code, look for examples (int/string) in GeometryDefaultExtensions.cpp
//
// Instructions:
//
// 1. Read the assumptions above and provide template initialisation if needed.
// 2. Add an alias to your type under these comments
// 3. Add a TYPESYSTEM_SOURCE_TEMPLATE_T in the cpp file to generate class type information
// 4. Provide a specialisation of getPyObject to generate a py object of the corresponding type (cpp
// file)
// 5. Provide specialisations if your type does not meet the assumptions above (e.g. for
// serialisation) (cpp file)
// 6. Register your type and corresponding python type in AppPart.cpp

template<typename T>
Base::Type GeometryDefaultExtension<T>::classTypeId {Base::Type::BadType};

// Must be explicitly declared here
template<>
void* GeometryDefaultExtension<long>::create();
template<>
void* GeometryDefaultExtension<std::string>::create();
template<>
void* GeometryDefaultExtension<bool>::create();
template<>
void* GeometryDefaultExtension<double>::create();

template<typename T>
inline GeometryDefaultExtension<T>::GeometryDefaultExtension()
    : value {}
{}

// Specialised constructors go here so that specialisation is before the template instantiation
// Specialised default constructors are inline, because a full specialisation otherwise shall go in
// the cpp file, but there it would be after the template instantiation.
template<>
inline GeometryDefaultExtension<long>::GeometryDefaultExtension()
    : value(0)
{}

template<>
inline GeometryDefaultExtension<double>::GeometryDefaultExtension()
    : value(0.0f)
{}

// Prefer alias to typedef item 9
using GeometryIntExtension = GeometryDefaultExtension<long>;
using GeometryStringExtension = GeometryDefaultExtension<std::string>;
using GeometryBoolExtension = GeometryDefaultExtension<bool>;
using GeometryDoubleExtension = GeometryDefaultExtension<double>;
}  // namespace Part

#endif  // PART_GEOMETRYDEFAULTEXTENSION_H
