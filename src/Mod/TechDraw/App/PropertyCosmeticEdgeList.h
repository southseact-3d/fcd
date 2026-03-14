#ifndef TECHDRAW_PropertyCosmeticEdgeList_H
#define TECHDRAW_PropertyCosmeticEdgeList_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <vector>
#include <App/Property.h>


namespace Base {
class Writer;
}

namespace TechDraw
{
class CosmeticEdge;

class TechDrawExport PropertyCosmeticEdgeList: public App::PropertyLists
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyCosmeticEdgeList();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~PropertyCosmeticEdgeList() override;

    void setSize(int newSize) override;
    int getSize(void) const override;

    /** Sets the property
     */
/*    void setValue(const CosmeticEdge*);*/
    void setValue(CosmeticEdge*);
    void setValues(const std::vector<CosmeticEdge*>&);

    /// index operator
    const CosmeticEdge *operator[] (const int idx) const {
        return _lValueList[idx];
    }

    const std::vector<CosmeticEdge*> &getValues() const {
        return _lValueList;
    }

    PyObject *getPyObject() override;
    void setPyObject(PyObject *) override;

    void Save(Base::Writer &writer) const override;
    void Restore(Base::XMLReader &reader) override;

    App::Property *Copy() const override;
    void Paste(const App::Property &from) override;

    unsigned int getMemSize(void) const override;

private:
    std::vector<CosmeticEdge*> _lValueList;
};

} // namespace TechDraw


#endif // TECHDRAW_PropertyCosmeticEdgeList_H
