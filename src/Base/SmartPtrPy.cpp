#include "SmartPtrPy.h"
#include "Interpreter.h"
#include <CXX/Objects.hxx>


namespace Py
{
void SmartPtr::set(PyObject* pyob, bool owned)
{
    release();
    p = pyob;
    if (!owned) {
        Py::_XINCREF(p);
    }
}

void SmartPtr::release()
{
    Base::PyGILStateLocker lock;
    Py::_XDECREF(p);
    p = nullptr;
}

SmartPtr::SmartPtr()
    : p(Py::_None())
{
    Py::_XINCREF(p);
}

SmartPtr::SmartPtr(PyObject* pyob, bool owned)
    : p(pyob)
{
    if (!owned) {
        Py::_XINCREF(p);
    }
}

SmartPtr::SmartPtr(const SmartPtr& ob)
    : p(ob.p)
{
    Py::_XINCREF(p);
}

SmartPtr& SmartPtr::operator=(const SmartPtr& rhs)
{
    set(rhs.ptr());
    return *this;
}

SmartPtr& SmartPtr::operator=(const Object& rhs)
{
    set(rhs.ptr());
    return *this;
}

SmartPtr& SmartPtr::operator=(PyObject* rhsp)
{
    if (ptr() != rhsp) {
        set(rhsp);
    }

    return *this;
}

SmartPtr::~SmartPtr()
{
    release();
}

PyObject* SmartPtr::operator*() const
{
    return p;
}

PyObject* SmartPtr::ptr() const
{
    return p;
}

bool SmartPtr::is(PyObject* pother) const
{  // identity test
    return p == pother;
}

bool SmartPtr::is(const SmartPtr& other) const
{  // identity test
    return p == other.p;
}

bool SmartPtr::isNull() const
{
    return p == nullptr;
}

BaseExport PyObject* new_reference_to(const SmartPtr& ptr)
{
    PyObject* py = ptr.ptr();
    Py::_XINCREF(py);
    return py;
}
}  // namespace Py
