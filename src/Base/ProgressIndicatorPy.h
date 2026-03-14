#ifndef BASE_PROGRESSINDICATORPY_H
#define BASE_PROGRESSINDICATORPY_H

#include <memory>
#include <CXX/Extensions.hxx>
#include <Base/Sequencer.h>

namespace Base
{
// NOLINTNEXTLINE
class BaseExport ProgressIndicatorPy: public Py::PythonExtension<ProgressIndicatorPy>
{
public:
    static void init_type();  // announce properties and methods
    static Py::PythonType& behaviors();
    static PyTypeObject* type_object();
    static bool check(PyObject* py);

    ProgressIndicatorPy();
    ~ProgressIndicatorPy() override;

    Py::Object repr() override;

    Py::Object start(const Py::Tuple&);
    Py::Object next(const Py::Tuple&);
    Py::Object stop(const Py::Tuple&);

private:
    static PyObject* PyMake(struct _typeobject*, PyObject*, PyObject*);

private:
    std::unique_ptr<SequencerLauncher> _seq;
};

}  // namespace Base

#endif  // BASE_PROGRESSINDICATORPY_H
