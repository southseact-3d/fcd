#ifndef MATERIAL_PROPERTYMATERIAL_H
#define MATERIAL_PROPERTYMATERIAL_H

#include <App/Property.h>
#include <Base/Reader.h>

#include "Materials.h"

namespace App
{
class Material;
}

namespace Materials
{

/** Material properties
 * This is the father of all properties handling colors.
 */
class MaterialsExport PropertyMaterial: public App::Property
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyMaterial();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~PropertyMaterial() override;

    /** Sets the property
     */
    void setValue(const Material& mat);

    /** Sets the appearance properties
     */
    void setValue(const App::Material& mat);

    /** This method returns a string representation of the property
     */
    const Material& getValue() const;

    PyObject* getPyObject() override;
    void setPyObject(PyObject*) override;

    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;

    const char* getEditorName() const override;

    Property* Copy() const override;
    void Paste(const Property& from) override;

    unsigned int getMemSize() const override
    {
        return sizeof(_material);
    }

    bool isSame(const Property& other) const override
    {
        if (&other == this) {
            return true;
        }
        return getTypeId() == other.getTypeId()
            && getValue() == static_cast<decltype(this)>(&other)->getValue();
    }

private:
    Material _material;
};

}  // namespace Materials

#endif  // MATERIAL_PROPERTYMATERIAL_H
