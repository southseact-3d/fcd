#include "CXX/WrapPython.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 13
PyObject py_object_initializer =
    {
    { 1 },
    NULL    // type must be init'ed by user
    };

#else
PyObject py_object_initializer =
    {
    _PyObject_EXTRA_INIT
    1,
    NULL    // type must be init'ed by user
    };

#endif

#ifdef __cplusplus
}
#endif
