#ifndef BASE_EXCEPTIONFACTORY_H
#define BASE_EXCEPTIONFACTORY_H


#include "Factory.h"
#include <FCGlobal.h>
#include <typeinfo>

// Python stuff
using PyObject = struct _object;

namespace Base
{

struct PyExceptionData
{
    PyObject* pyexc {};
    std::string message;
    bool reported {};
};

/// Abstract base class of all exception producers
class BaseExport AbstractExceptionProducer: public AbstractProducer
{
public:
    AbstractExceptionProducer() = default;
    // just implement it
    void* Produce() const override
    {
        return nullptr;
    }
    virtual void raiseException(PyObject* pydict) const = 0;
    virtual void raiseExceptionByType(const PyExceptionData&) const = 0;
};

// --------------------------------------------------------------------

/** The ExceptionFactory */
class BaseExport ExceptionFactory: public Factory
{
public:
    static ExceptionFactory& Instance();
    static void Destruct();

    void raiseException(PyObject* pydict) const;
    void raiseExceptionByType(const PyExceptionData& data) const;

private:
    static ExceptionFactory* _pcSingleton;  // NOLINT

    ExceptionFactory() = default;
};

/* Producers */

template<class CLASS>
class ExceptionProducer: public AbstractExceptionProducer
{
public:
    ExceptionProducer()
    {
        CLASS cls;
        pyExcType = cls.getPyExceptionType();
        ExceptionFactory::Instance().AddProducer(typeid(CLASS).name(), this);
    }

    void raiseException(PyObject* pydict) const override
    {
        CLASS cls;
        cls.setPyObject(pydict);

        throw cls;
    }

    void raiseExceptionByType(const PyExceptionData& data) const override
    {
        if (pyExcType == data.pyexc) {
            CLASS cls;
            cls.setMessage(data.message);
            cls.setReported(data.reported);

            throw cls;
        }
    }

private:
    PyObject* pyExcType {};
};

}  // namespace Base


#endif
