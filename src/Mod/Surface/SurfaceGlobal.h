#include <FCGlobal.h>

#ifndef SURFACE_GLOBAL_H
# define SURFACE_GLOBAL_H


// Surface
# ifndef SurfaceExport
#  ifdef Surface_EXPORTS
#   define SurfaceExport FREECAD_DECL_EXPORT
#  else
#   define SurfaceExport FREECAD_DECL_IMPORT
#  endif
# endif

// SurfaceGui
# ifndef SurfaceGuiExport
#  ifdef SurfaceGui_EXPORTS
#   define SurfaceGuiExport FREECAD_DECL_EXPORT
#  else
#   define SurfaceGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // SURFACE_GLOBAL_H
