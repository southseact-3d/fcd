#ifndef SANDBOX_DOCUMENTPROTECTORPY_H
#define SANDBOX_DOCUMENTPROTECTORPY_H

#include <CXX/Extensions.hxx>

namespace App {
class DocumentPy;
class DocumentObject;
class DocumentObjectPy;
}

namespace Sandbox {

class DocumentProtector;
class DocumentProtectorPy : public Py::PythonExtension<DocumentProtectorPy>
{
public:
    static void init_type(void);    // announce properties and methods

    DocumentProtectorPy(App::DocumentPy *doc);
    ~DocumentProtectorPy();

    Py::Object repr();
    Py::Object getattr(const char *);
    int setattr(const char *, const Py::Object &);

    Py::Object addObject(const Py::Tuple&);
    Py::Object recompute(const Py::Tuple&);

private:
    using method_varargs_handler = PyObject* (*)(PyObject *_self, PyObject *_args);
    static method_varargs_handler pycxx_handler;
    static PyObject *method_varargs_ext_handler(PyObject *_self, PyObject *_args);

private:
    DocumentProtector* _dp;
    friend class DocumentProtector;
};

class DocumentObjectProtector;
class DocumentObjectProtectorPy : public Py::PythonExtension<DocumentObjectProtectorPy>
{
public:
    static void init_type(void);    // announce properties and methods

    DocumentObjectProtectorPy(App::DocumentObject *obj);
    DocumentObjectProtectorPy(App::DocumentObjectPy *obj);
    ~DocumentObjectProtectorPy();

    Py::Object repr();
    Py::Object getattr(const char *);
    Py::Object getObject() const;
    int setattr(const char *, const Py::Object &);
    Py::Object purgeTouched(const Py::Tuple&);

private:
    DocumentObjectProtector* _dp;
    friend class DocumentObjectProtector;
};

}

#endif // SANDBOX_DOCUMENTPROTECTORPY_H

