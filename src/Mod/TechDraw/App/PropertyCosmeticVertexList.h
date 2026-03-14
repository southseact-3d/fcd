#ifndef TECHDRAW_PropertyCosmeticVertexList_H
#define TECHDRAW_PropertyCosmeticVertexList_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <vector>
#include <App/Property.h>

#include "CosmeticVertex.h"


namespace Base {
class Writer;
}

namespace TechDraw
{
class CosmeticVertex;

class TechDrawExport PropertyCosmeticVertexList: public App::PropertyLists
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyCosmeticVertexList();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~PropertyCosmeticVertexList() override;

    void setSize(int newSize) override;
    int getSize(void) const override;

    /** Sets the property
     */
    void setValue(CosmeticVertex*);
    void setValues(const std::vector<CosmeticVertex*>&);

    /// index operator
    const CosmeticVertex *operator[] (const int idx) const {
        return _lValueList[idx];
    }

    const std::vector<CosmeticVertex*> &getValues() const {
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
    std::vector<CosmeticVertex*> _lValueList;
};

} // namespace TechDraw


#endif // TECHDRAW_PropertyCosmeticVertexList_H
