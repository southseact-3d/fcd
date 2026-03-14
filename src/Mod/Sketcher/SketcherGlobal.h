#include <FCGlobal.h>

#ifndef SKETCHER_GLOBAL_H
# define SKETCHER_GLOBAL_H


// Sketcher
# ifndef SketcherExport
#  ifdef Sketcher_EXPORTS
#   define SketcherExport FREECAD_DECL_EXPORT
#  else
#   define SketcherExport FREECAD_DECL_IMPORT
#  endif
# endif

// SketcherGui
# ifndef SketcherGuiExport
#  ifdef SketcherGui_EXPORTS
#   define SketcherGuiExport FREECAD_DECL_EXPORT
#  else
#   define SketcherGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // SKETCHER_GLOBAL_H
