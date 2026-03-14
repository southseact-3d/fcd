#ifndef TESTGUI_UNITTESTPY_H
#define TESTGUI_UNITTESTPY_H

#include <Base/PyObjectBase.h>
#include <CXX/Extensions.hxx>


namespace TestGui
{

class UnitTestDialog;
class UnitTestDialogPy: public Py::PythonExtension<UnitTestDialogPy>
{
public:
    static void init_type();  // announce properties and methods

    UnitTestDialogPy();
    ~UnitTestDialogPy() override;

    Py::Object repr() override;
    Py::Object getattr(const char*) override;
    int setattr(const char*, const Py::Object&) override;

    Py::Object clearErrorList(const Py::Tuple&);
    Py::Object insertError(const Py::Tuple&);
    Py::Object setUnitTest(const Py::Tuple&);
    Py::Object getUnitTest(const Py::Tuple&);
    Py::Object setStatusText(const Py::Tuple&);
    Py::Object setProgressFrac(const Py::Tuple&);
    Py::Object errorDialog(const Py::Tuple&);
    Py::Object setRunCount(const Py::Tuple&);
    Py::Object setFailCount(const Py::Tuple&);
    Py::Object setErrorCount(const Py::Tuple&);
    Py::Object setRemainCount(const Py::Tuple&);
    Py::Object updateGUI(const Py::Tuple&);
    Py::Object addUnitTest(const Py::Tuple&);
    Py::Object clearUnitTests(const Py::Tuple&);

private:
    using method_varargs_handler = PyObject* (*)(PyObject * _self, PyObject* _args);
    static method_varargs_handler pycxx_handler;
    static PyObject* method_varargs_ext_handler(PyObject* _self, PyObject* _args);
};

}  // namespace TestGui


#endif
