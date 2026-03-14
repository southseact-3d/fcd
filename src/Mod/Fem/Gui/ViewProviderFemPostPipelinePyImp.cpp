#include "ViewProviderFemPostPipeline.h"
// inclusion of the generated files (generated out of ViewProviderFemPostPipelinePy.xml)
#include "ViewProviderFemPostPipelinePy.h"
#include "ViewProviderFemPostPipelinePy.cpp"
// clang-format on


using namespace FemGui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderFemPostPipelinePy::representation() const
{
    return {"<ViewProviderFemPostPipeline object>"};
}

PyObject* ViewProviderFemPostPipelinePy::updateColorBars(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    this->getViewProviderFemPostPipelinePtr()->updateColorBars();

    Py_Return;
}

PyObject* ViewProviderFemPostPipelinePy::transformField(PyObject* args)
{
    char* FieldName;
    double FieldFactor;

    if (!PyArg_ParseTuple(args, "sd", &FieldName, &FieldFactor)) {
        return nullptr;
    }

    this->getViewProviderFemPostPipelinePtr()->transformField(FieldName, FieldFactor);

    Py_Return;
}

PyObject* ViewProviderFemPostPipelinePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderFemPostPipelinePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
