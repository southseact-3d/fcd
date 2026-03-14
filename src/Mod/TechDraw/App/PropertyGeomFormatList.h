#ifndef TECHDRAW_PropertyGeomFormatList_H
#define TECHDRAW_PropertyGeomFormatList_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <vector>
#include <App/Property.h>

namespace Base {
class Writer;
}

namespace TechDraw
{
class GeomFormat;

class TechDrawExport PropertyGeomFormatList: public App::PropertyLists
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyGeomFormatList();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~PropertyGeomFormatList() override;

    void setSize(int newSize) override;
    int getSize() const override;

    /** Sets the property
     */
    void setValue(const GeomFormat*);
    void setValues(const std::vector<GeomFormat*>&);

    /// index operator
    const GeomFormat *operator[] (const int idx) const {
        return _lValueList[idx];
    }

    const std::vector<GeomFormat*> &getValues() const {
        return _lValueList;
    }

    PyObject *getPyObject() override;
    void setPyObject(PyObject *) override;

    void Save(Base::Writer &writer) const override;
    void Restore(Base::XMLReader &reader) override;

    App::Property *Copy() const override;
    void Paste(const App::Property &from) override;

    unsigned int getMemSize() const override;

private:
    std::vector<GeomFormat*> _lValueList;
};

} // namespace TechDraw


#endif // TECHDRAW_PropertyGeomFormatList_H
