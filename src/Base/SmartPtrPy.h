#ifndef PY_SMARTPTRPY_H
#define PY_SMARTPTRPY_H

#include <FCGlobal.h>

// forward declarations
using PyObject = struct _object;

namespace Py
{
class Object;

/**
 * \brief This is a stripped-down version of Py::Object.
 * The purpose of this class is to avoid to include any other header file
 * and therefore the PyObject is forward declared and the implementation
 * is separated from the class declaration.
 *
 * \author Werner Mayer
 */
class BaseExport SmartPtr  // NOLINT
{
private:
    PyObject* p;

protected:
    void set(PyObject* pyob, bool owned = false);
    void release();

public:
    SmartPtr();

    // Constructor acquires new ownership of pointer unless explicitly told not to.
    explicit SmartPtr(PyObject* pyob, bool owned = false);

    // Copy constructor acquires new ownership of pointer
    SmartPtr(const SmartPtr& ob);

    // Assignment acquires new ownership of pointer
    SmartPtr& operator=(const SmartPtr& rhs);
    SmartPtr& operator=(const Object& rhs);

    SmartPtr& operator=(PyObject* rhsp);

    // Destructor
    virtual ~SmartPtr();

    // Loaning the pointer to others, retain ownership
    PyObject* operator*() const;

    // Would like to call this pointer() but messes up STL in SeqBase<T>
    PyObject* ptr() const;
    //
    // Queries
    //

    bool is(PyObject* pother) const;
    bool is(const SmartPtr& other) const;

    bool isNull() const;
};

BaseExport PyObject* new_reference_to(const SmartPtr&);

}  // namespace Py

#endif  // PY_SMARTPTRPY_H
