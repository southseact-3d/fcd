#include <QMetaType>
#include <QUuid>



#include <App/Application.h>
#include <Base/Writer.h>
#include <Gui/MetaTypes.h>

#include "MaterialManager.h"
#include "MaterialPy.h"
#include "PropertyMaterial.h"

using namespace Materials;

/* TRANSLATOR Material::PropertyMaterial */

TYPESYSTEM_SOURCE(Materials::PropertyMaterial, App::Property)

PropertyMaterial::PropertyMaterial() = default;

PropertyMaterial::~PropertyMaterial() = default;

void PropertyMaterial::setValue(const Material& mat)
{
    aboutToSetValue();
    _material = mat;
    hasSetValue();
}

void PropertyMaterial::setValue(const App::Material& mat)
{
    aboutToSetValue();
    _material = mat;
    hasSetValue();
}

const Material& PropertyMaterial::getValue() const
{
    return _material;
}

PyObject* PropertyMaterial::getPyObject()
{
    return new MaterialPy(new Material(_material));
}

void PropertyMaterial::setPyObject(PyObject* value)
{
    if (PyObject_TypeCheck(value, &(MaterialPy::Type))) {
        setValue(*static_cast<MaterialPy*>(value)->getMaterialPtr());
    }
    else {
        std::string error = std::string("type must be 'Material' not ");
        error += value->ob_type->tp_name;
        throw Base::TypeError(error);
    }
}

void PropertyMaterial::Save(Base::Writer& writer) const
{
    writer.Stream() << writer.ind() << "<PropertyMaterial uuid=\""
                    << _material.getUUID().toStdString() << "\"/>" << std::endl;
}

void PropertyMaterial::Restore(Base::XMLReader& reader)
{
    // read my Element
    reader.readElement("PropertyMaterial");
    // get the value of my Attribute
    auto uuid = reader.getAttribute<const char*>("uuid");

    setValue(*MaterialManager::getManager().getMaterial(QString::fromLatin1(uuid)));
}

const char* PropertyMaterial::getEditorName() const
{
    if (testStatus(MaterialEdit)) {
        return "";  //"Gui::PropertyEditor::PropertyMaterialItem";
    }
    return "";
}

App::Property* PropertyMaterial::Copy() const
{
    PropertyMaterial* p = new PropertyMaterial();
    p->_material = _material;
    return p;
}

void PropertyMaterial::Paste(const App::Property& from)
{
    aboutToSetValue();
    _material = dynamic_cast<const PropertyMaterial&>(from)._material;
    hasSetValue();
}
