#include <FCGlobal.h>

#ifndef FEM_GLOBAL_H
# define FEM_GLOBAL_H


// Fem
# ifndef FemExport
#  ifdef Fem_EXPORTS
#   define FemExport FREECAD_DECL_EXPORT
#  else
#   define FemExport FREECAD_DECL_IMPORT
#  endif
# endif

// FemGui
# ifndef FemGuiExport
#  ifdef FemGui_EXPORTS
#   define FemGuiExport FREECAD_DECL_EXPORT
#  else
#   define FemGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // FEM_GLOBAL_H
