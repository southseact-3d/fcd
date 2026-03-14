#ifndef TECHDRAW_PropertyCenterLineList_H
#define TECHDRAW_PropertyCenterLineList_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <vector>
#include <App/Property.h>


namespace Base {
class Writer;
}

namespace TechDraw
{
class CenterLine;

class TechDrawExport PropertyCenterLineList: public App::PropertyLists
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyCenterLineList();
    ~PropertyCenterLineList() override;

    void setSize(int newSize) override;
    int getSize() const override;

    /** Sets the property
     */
    void setValue(CenterLine*);
    void setValues(const std::vector<CenterLine*>&);

    /// index operator
    const CenterLine *operator[] (const int idx) const {
        return _lValueList[idx];
    }

    const std::vector<CenterLine*> &getValues() const {
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
    std::vector<CenterLine*> _lValueList;
};

} // namespace TechDraw


#endif // TECHDRAW_PropertyCenterLineList_H
