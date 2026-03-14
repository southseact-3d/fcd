// Public header for FT2FC.cpp
#ifndef FT2FC_H
#define FT2FC_H
// public functions
PyObject* FT2FC(
    const Py_UCS4* unichars,
    const size_t length,
    const char* FontPath,
    const char* FontName,
    const double stringheight,
    const double tracking
);

PyObject* FT2FC(
    const Py_UCS4* unichars,
    const size_t length,
    const char* FontSpec,
    const double stringheight,
    const double tracking
);

#endif  // FT2FC_H
