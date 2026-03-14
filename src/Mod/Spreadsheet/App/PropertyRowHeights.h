#ifndef PROPERTYROWHEIGHTS_H
#define PROPERTYROWHEIGHTS_H

#include <App/Property.h>
#include <CXX/Objects.hxx>
#include <Mod/Spreadsheet/SpreadsheetGlobal.h>
#include <map>

namespace Spreadsheet
{

class SpreadsheetExport PropertyRowHeights: public App::Property, std::map<int, int>
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyRowHeights();

    void setValue()
    {}

    void setValue(int row, int height);

    void setValues(const std::map<int, int>&);

    int getValue(int row) const
    {
        std::map<int, int>::const_iterator i = find(row);
        return i != end() ? i->second : defaultHeight;
    }

    std::map<int, int> getValues() const
    {
        return *this;
    }

    App::Property* Copy() const override;

    void Paste(const App::Property& from) override;

    void Save(Base::Writer& writer) const override;

    void Restore(Base::XMLReader& reader) override;

    bool isDirty() const
    {
        return dirty.size() > 0;
    }

    void clearDirty()
    {
        dirty.clear();
    }

    const std::set<int>& getDirty() const
    {
        return dirty;
    }

    PyObject* getPyObject() override;

    static const int defaultHeight;

    void clear();

private:
    PropertyRowHeights(const PropertyRowHeights& other);

    std::set<int> dirty;

    Py::Object PythonObject;
};

}  // namespace Spreadsheet

#endif  // PROPERTYROWHEIGHTS_H
