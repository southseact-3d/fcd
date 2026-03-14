#ifndef APP_FEATUREPYTHONPYIMP_H
#define APP_FEATUREPYTHONPYIMP_H

#include <Base/BaseClass.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wmissing-field-initializers"
#elif defined(__GNUC__) || defined(__GNUG__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define PYTHON_TYPE_DEF(_class_, _subclass_) \
    class _class_ : public _subclass_ \
    { \
    public: \
        static PyTypeObject Type; \
    public: \
        _class_(Base::BaseClass *pcObject, PyTypeObject *T = &Type); \
        virtual ~_class_(); \
    };

#if PY_VERSION_HEX >= 0x030d0000
#define PYTHON_TYPE_SLOTS 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#elif PY_VERSION_HEX >= 0x030c0000
#define PYTHON_TYPE_SLOTS 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#else
#define PYTHON_TYPE_SLOTS 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

#endif \

#define PYTHON_TYPE_IMP(_class_, _subclass_) \
    PyTypeObject _class_::Type = { \
        PyVarObject_HEAD_INIT(&PyType_Type, 0) \
        ""#_class_"",  \
        sizeof(_class_),  \
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
        Py_TPFLAGS_BASETYPE|Py_TPFLAGS_DEFAULT, \
        ""#_class_"", \
        0, 0, 0, 0, 0, 0, 0, 0, 0, \
        &_subclass_::Type, \
        PYTHON_TYPE_SLOTS \
    }; \
    _class_::_class_(Base::BaseClass *pcObject, PyTypeObject *T) \
        : _subclass_(reinterpret_cast<_subclass_::PointerType>(pcObject), T) \
    { \
    } \
    _class_::~_class_() \
    { \
    }

namespace App
{

class Property;

/**
 * @author Werner Mayer
 */
template <class FeaturePyT>
class FeaturePythonPyT : public FeaturePyT
{
public:
    static PyTypeObject   Type;

public:
    explicit FeaturePythonPyT(Base::BaseClass *pcObject, PyTypeObject *T = &Type);
    ~FeaturePythonPyT() override;

    /** @name callbacks and implementers for the python object methods */
    //@{
    static  int __setattro(PyObject *PyObj, PyObject *attro, PyObject *value);
    //@}
    PyObject *_getattr(const char *attr) override;
    int _setattr(const char *attr, PyObject *value) override;

protected:
    PyObject * dict_methods;

private:
};

} //namespace App

#include "FeaturePythonPyImp.inl"

#if defined(__clang__)
# pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
# pragma GCC diagnostic pop
#endif

#endif // APP_FEATUREPYTHONPYIMP_H
// clang-format on
