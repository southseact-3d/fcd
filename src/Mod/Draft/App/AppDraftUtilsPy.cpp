#include <Base/Console.h>
#include <Base/Interpreter.h>


namespace DraftUtils
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("DraftUtils")
    {
        add_varargs_method(
            "readDXF",
            &Module::readDXF,
            "readDXF(filename,[document,ignore_errors]): "
            "Imports a DXF file into the given document. "
            "ignore_errors is True by default. "
            "NOTE: DraftUtils.readDXF is removed. "
            "Use Import.readDxf instead."
        );
        initialize(
            "The DraftUtils module contains utility functions for the Draft module."
        );  // register
            // with
            // Python
    }

private:
    Py::Object readDXF(const Py::Tuple& /*args*/)
    {
        Base::Console().warning(
            "DraftUtils.readDXF is removed. "
            "Use Import.readDxf instead.\n"
        );
        return Py::None();
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace DraftUtils
