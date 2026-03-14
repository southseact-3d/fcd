#include "ExceptionFactory.h"
#include <CXX/Objects.hxx>


using namespace Base;

ExceptionFactory* ExceptionFactory::_pcSingleton = nullptr;  // NOLINT

ExceptionFactory& ExceptionFactory::Instance()
{
    if (!_pcSingleton) {
        _pcSingleton = new ExceptionFactory;  // NOLINT
    }
    return *_pcSingleton;
}

void ExceptionFactory::Destruct()
{
    delete _pcSingleton;
    _pcSingleton = nullptr;
}

void ExceptionFactory::raiseException(PyObject* pydict) const
{
    std::string classname;

    Py::Dict edict(pydict);
    if (edict.hasKey("sclassname")) {
        classname = static_cast<std::string>(Py::String(edict.getItem("sclassname")));

        auto pProd = _mpcProducers.find(classname);
        if (pProd != _mpcProducers.end()) {
            static_cast<AbstractExceptionProducer*>(pProd->second)->raiseException(pydict);
        }
    }
}

void ExceptionFactory::raiseExceptionByType(const PyExceptionData& data) const
{
    for (const auto& it : _mpcProducers) {
        static_cast<AbstractExceptionProducer*>(it.second)->raiseExceptionByType(data);  // NOLINT
    }
}
