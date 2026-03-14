#ifndef APP_PropertyGeometryList_H
#define APP_PropertyGeometryList_H

#include <vector>

#include <App/Property.h>

#include <Mod/Part/PartGlobal.h>

#include "Geometry.h"


namespace Base
{
class Writer;
}

namespace Part
{
class Geometry;

class PartExport PropertyGeometryList: public App::PropertyLists
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyGeometryList();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~PropertyGeometryList() override;

    void setSize(int newSize) override;
    int getSize() const override;

    /** Sets the property
     */
    void setValue(const Geometry*);
    void setValues(const std::vector<Geometry*>&);
    void setValues(std::vector<Geometry*>&&);

    void moveValues(PropertyGeometryList&& other);

    /// index operator
    Geometry* operator[](const int idx) const
    {
        return _lValueList[idx];
    }

    const std::vector<Geometry*>& getValues() const
    {
        return _lValueList;
    }

    void set1Value(int idx, std::unique_ptr<Geometry>&&);

    PyObject* getPyObject() override;
    void setPyObject(PyObject*) override;

    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;

    App::Property* Copy() const override;
    void Paste(const App::Property& from) override;

    unsigned int getMemSize() const override;

private:
    void trySaveGeometry(Geometry* geom, Base::Writer& writer) const;
    void tryRestoreGeometry(Geometry* geom, Base::XMLReader& reader);

private:
    std::vector<Geometry*> _lValueList;
};

}  // namespace Part


#endif  // APP_PropertyGeometryList_H
