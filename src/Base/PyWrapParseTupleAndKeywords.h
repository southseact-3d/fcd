#include <Python.h>
#include <array>

namespace Base
{

/// A thin C++ wrapper around PyArg_ParseTupleAndKeywords providing const-correctness for the
/// keywords argument \arg args The Python args object \arg kw The Python kw object \arg format The
/// format string describing the expected arguments \arg keywords A std::array of keywords,
/// terminated by a nullptr (required by CPython) \arg (variadic) Pointers to the storage locations
/// for the parameters \returns boolean true on success, or false on failure
template<size_t arraySize>
bool Wrapped_ParseTupleAndKeywords(
    PyObject* args,
    PyObject* kw,
    const char* format,
    const std::array<const char*, arraySize> keywords,
    ...
)
{
    static_assert(arraySize > 0, "keywords array must have at least a single nullptr in it");
    if (keywords.back()) {
        PyErr_SetString(PyExc_ValueError, "Last element of keywords array is not null");
        return false;
    }

    // NOTE: This code is from getargs.c in the Python source code (modified to use the public
    // interface at PyArg_VaParseTupleAndKeywords and to return a bool).

    if ((args == nullptr || !PyTuple_Check(args)) || (kw != nullptr && !PyDict_Check(kw))
        || format == nullptr) {
        PyErr_BadInternalCall();
        return false;
    }

    va_list va;  // NOLINT
    va_start(va, keywords);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    int retval
        = PyArg_VaParseTupleAndKeywords(args, kw, format, const_cast<char**>(keywords.data()), va);
    va_end(va);
    return retval != 0;  // Convert to a true C++ boolean
}

}  // namespace Base

#endif  // FREECAD_PYWRAPPARSETUPLEANDKEYWORDS_H
