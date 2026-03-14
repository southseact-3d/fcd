#include <FCGlobal.h>

#ifndef POINTS_GLOBAL_H
# define POINTS_GLOBAL_H


// Points
# ifndef PointsExport
#  ifdef Points_EXPORTS
#   define PointsExport FREECAD_DECL_EXPORT
#  else
#   define PointsExport FREECAD_DECL_IMPORT
#  endif
# endif

// PointsGui
# ifndef PointsGuiExport
#  ifdef PointsGui_EXPORTS
#   define PointsGuiExport FREECAD_DECL_EXPORT
#  else
#   define PointsGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // POINTS_GLOBAL_H
