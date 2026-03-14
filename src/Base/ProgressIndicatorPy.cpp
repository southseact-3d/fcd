#include "Exception.h"
#include "ProgressIndicatorPy.h"


using namespace Base;

void ProgressIndicatorPy::init_type()
{
    behaviors().name("ProgressIndicator");
    behaviors().doc("Progress indicator");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    behaviors().supportGetattr();
    behaviors().supportSetattr();
    behaviors().set_tp_new(PyMake);

    add_varargs_method("start", &ProgressIndicatorPy::start, "start(string,int)");
    add_varargs_method("next", &ProgressIndicatorPy::next, "next()");
    add_varargs_method("stop", &ProgressIndicatorPy::stop, "stop()");
}

Py::PythonType& ProgressIndicatorPy::behaviors()
{
    return Py::PythonExtension<ProgressIndicatorPy>::behaviors();
}

PyTypeObject* ProgressIndicatorPy::type_object()
{
    return Py::PythonExtension<ProgressIndicatorPy>::type_object();
}

bool ProgressIndicatorPy::check(PyObject* py)
{
    return Py::PythonExtension<ProgressIndicatorPy>::check(py);
}

PyObject* ProgressIndicatorPy::PyMake(PyTypeObject* /*unused*/, PyObject* /*unused*/, PyObject* /*unused*/)
{
    return new ProgressIndicatorPy();
}

ProgressIndicatorPy::ProgressIndicatorPy() = default;

ProgressIndicatorPy::~ProgressIndicatorPy() = default;

Py::Object ProgressIndicatorPy::repr()
{
    return Py::String("Base.ProgressIndicator");
}

Py::Object ProgressIndicatorPy::start(const Py::Tuple& args)
{
    char* text = nullptr;
    unsigned int steps = 0;
    if (!PyArg_ParseTuple(args.ptr(), "sI", &text, &steps)) {
        throw Py::Exception();
    }
    if (!_seq) {
        _seq = std::make_unique<SequencerLauncher>(text, steps);
    }
    return Py::None();
}

Py::Object ProgressIndicatorPy::next(const Py::Tuple& args)
{
    int b = 0;
    if (!PyArg_ParseTuple(args.ptr(), "|i", &b)) {
        throw Py::Exception();
    }
    if (_seq) {
        try {
            _seq->next(b != 0);
        }
        catch (const Base::AbortException&) {
            _seq.reset();
            throw Py::RuntimeError("abort progress indicator");
        }
    }
    return Py::None();
}

Py::Object ProgressIndicatorPy::stop(const Py::Tuple& args)
{
    if (!PyArg_ParseTuple(args.ptr(), "")) {
        throw Py::Exception();
    }
    _seq.reset();
    return Py::None();
}
