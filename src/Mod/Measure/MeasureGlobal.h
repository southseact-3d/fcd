#include <FCGlobal.h>

#ifndef MEASURE_GLOBAL_H
# define MEASURE_GLOBAL_H


// Measure
# ifndef MeasureExport
#  ifdef Measure_EXPORTS
#   define MeasureExport FREECAD_DECL_EXPORT
#  else
#   define MeasureExport FREECAD_DECL_IMPORT
#  endif
# endif

// MeasureGui
# ifndef MeasureGuiExport
#  ifdef MeasureGui_EXPORTS
#   define MeasureGuiExport FREECAD_DECL_EXPORT
#  else
#   define MeasureGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // MEASURE_GLOBAL_H
