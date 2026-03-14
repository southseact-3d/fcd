#include <sstream>


#include <App/DocumentObject.h>
#include <App/PropertyContainer.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "PropertyPath.h"
#include "PathPy.h"


using namespace Path;

TYPESYSTEM_SOURCE(Path::PropertyPath, App::Property)

PropertyPath::PropertyPath()
{}

PropertyPath::~PropertyPath()
{}

void PropertyPath::setValue(const Toolpath& pa)
{
    aboutToSetValue();
    _Path = pa;
    hasSetValue();
}


const Toolpath& PropertyPath::getValue() const
{
    return _Path;
}

PyObject* PropertyPath::getPyObject()
{
    return new PathPy(new Toolpath(_Path));
}

void PropertyPath::setPyObject(PyObject* value)
{
    if (PyObject_TypeCheck(value, &(PathPy::Type))) {
        PathPy* pcObject = static_cast<PathPy*>(value);
        setValue(*pcObject->getToolpathPtr());
    }
    else {
        std::string error = std::string("type must be 'Path', not ");
        error += value->ob_type->tp_name;
        throw Base::TypeError(error);
    }
}

App::Property* PropertyPath::Copy() const
{
    PropertyPath* prop = new PropertyPath();
    prop->_Path = this->_Path;

    return prop;
}

void PropertyPath::Paste(const App::Property& from)
{
    aboutToSetValue();
    _Path = dynamic_cast<const PropertyPath&>(from)._Path;
    hasSetValue();
}

unsigned int PropertyPath::getMemSize() const
{
    return _Path.getMemSize();
}

void PropertyPath::Save(Base::Writer& writer) const
{
    _Path.Save(writer);
}

void PropertyPath::Restore(Base::XMLReader& reader)
{
    reader.readElement("Path");

    std::string file(reader.getAttribute<const char*>("file"));
    if (!file.empty()) {
        // initiate a file read
        reader.addFile(file.c_str(), this);
    }

    if (reader.hasAttribute("version")) {
        int version = reader.getAttribute<long>("version");
        if (version >= Toolpath::SchemaVersion) {
            reader.readElement("Center");
            double x = reader.getAttribute<double>("x");
            double y = reader.getAttribute<double>("y");
            double z = reader.getAttribute<double>("z");
            Base::Vector3d center(x, y, z);
            _Path.setCenter(center);
        }
    }
}

void PropertyPath::SaveDocFile(Base::Writer&) const
{
    // does nothing
}

void PropertyPath::RestoreDocFile(Base::Reader& reader)
{
    App::PropertyContainer* container = getContainer();
    App::DocumentObject* obj = nullptr;
    if (container->isDerivedFrom<App::DocumentObject>()) {
        obj = static_cast<App::DocumentObject*>(container);
    }

    if (obj) {
        obj->setStatus(App::ObjectStatus::Restore, true);
    }

    aboutToSetValue();
    _Path.RestoreDocFile(reader);
    hasSetValue();

    if (obj) {
        obj->setStatus(App::ObjectStatus::Restore, false);
    }
}
