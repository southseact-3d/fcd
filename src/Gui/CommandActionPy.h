#ifndef GUI_COMMANDACTIONPY_H
#define GUI_COMMANDACTIONPY_H

#include <CXX/Extensions.hxx>


namespace Gui
{
class Command;

class CommandActionPy: public Py::PythonClass<CommandActionPy>
{
public:
    static void init_type();

    CommandActionPy(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kdws);
    ~CommandActionPy() override;

    Py::Object getCommand();

protected:
    static PyObject* descriptorGetter(PyObject* self, PyObject* obj, PyObject* type);
    static int descriptorSetter(PyObject* self, PyObject* obj, PyObject* value);
    Py::Object repr() override;
    Py::Object getattro(const Py::String& attr) override;
    int setattro(const Py::String& attr_, const Py::Object& value) override;

    Py::Object getAction();

private:
    std::string cmdName;
    Command* cmd = nullptr;
};

}  // namespace Gui

#endif  // GUI_COMMANDACTIONPY_H
