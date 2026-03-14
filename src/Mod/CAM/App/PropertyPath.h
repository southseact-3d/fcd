#ifndef PROPERTYPATH_H
#define PROPERTYPATH_H

#include <App/Property.h>

#include "Path.h"


namespace Path
{

/** The path property class.  */
class PathExport PropertyPath: public App::Property
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    PropertyPath();
    ~PropertyPath() override;

    /** @name Getter/setter */
    //@{
    /// set the part shape
    void setValue(const Toolpath&);
    /// get the part shape
    const Toolpath& getValue() const;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject() override;
    void setPyObject(PyObject* value) override;
    //@}

    /** @name Save/restore */
    //@{
    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;
    void SaveDocFile(Base::Writer& writer) const override;
    void RestoreDocFile(Base::Reader& reader) override;

    App::Property* Copy() const override;
    void Paste(const App::Property& from) override;
    unsigned int getMemSize() const override;
    //@}

private:
    Toolpath _Path;
};


}  // namespace Path


#endif  // PROPERTYPATH_H
