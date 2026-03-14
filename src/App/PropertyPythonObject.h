#ifndef APP_PROPERTYPYTHONOBJECT_H
#define APP_PROPERTYPYTHONOBJECT_H

#include <string>
#include <CXX/Objects.hxx>

#include "Property.h"


namespace Base
{
class Writer;
class XMLReader;
}  // namespace Base

namespace App
{

/**
 * PropertyPythonObject is used to manage Py::Object instances as properties.
 * @author Werner Mayer
 */
class AppExport PropertyPythonObject: public Property
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyPythonObject();
    ~PropertyPythonObject() override;

    void setValue(const Py::Object& py);
    Py::Object getValue() const;

    PyObject* getPyObject() override;
    void setPyObject(PyObject* obj) override;

    /** Use Python's pickle module to save the object */
    void Save(Base::Writer& writer) const override;
    /** Use Python's pickle module to restore the object */
    void Restore(Base::XMLReader& reader) override;
    void SaveDocFile(Base::Writer& writer) const override;
    void RestoreDocFile(Base::Reader& reader) override;

    unsigned int getMemSize() const override;
    Property* Copy() const override;
    void Paste(const Property& from) override;

    std::string toString() const;
    void fromString(const std::string&);

private:
    void saveObject(Base::Writer& writer) const;
    void restoreObject(Base::XMLReader& reader);
    std::string encodeValue(const std::string& str) const;
    std::string decodeValue(const std::string& str) const;
    void loadPickle(const std::string& str);
    Py::Object object;
};


}  // namespace App

#endif  // APP_PROPERTYPYTHONOBJECT_H
