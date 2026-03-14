#ifndef PROPERTYCOLUMNWIDTHS_H
#define PROPERTYCOLUMNWIDTHS_H

#include <App/Property.h>
#include <CXX/Objects.hxx>
#include <Mod/Spreadsheet/SpreadsheetGlobal.h>
#include <map>

namespace Spreadsheet
{

class SpreadsheetExport PropertyColumnWidths: public App::Property, std::map<int, int>
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyColumnWidths();

    void setValue()
    {}

    void setValue(int col, int width);

    void setValues(const std::map<int, int>&);

    std::map<int, int> getValues() const
    {
        return *this;
    }

    int getValue(int column) const
    {
        std::map<int, int>::const_iterator i = find(column);
        return i != end() ? i->second : defaultWidth;
    }

    Property* Copy() const override;

    void Paste(const Property& from) override;

    void Save(Base::Writer& writer) const override;

    void Restore(Base::XMLReader& reader) override;

    bool isDirty() const
    {
        return !dirty.empty();
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

    void clear();

    static const int defaultWidth;
    static const int defaultHeaderWidth;

private:
    PropertyColumnWidths(const PropertyColumnWidths& other);

    std::set<int> dirty;

    Py::Object PythonObject;
};

}  // namespace Spreadsheet

#endif  // PROPERTYCOLUMNWIDTHS_H
