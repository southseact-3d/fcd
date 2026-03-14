#include <FCGlobal.h>

#ifndef MESH_GLOBAL_H
# define MESH_GLOBAL_H


// Mesh
# ifndef MeshExport
#  ifdef Mesh_EXPORTS
#   define MeshExport FREECAD_DECL_EXPORT
#  else
#   define MeshExport FREECAD_DECL_IMPORT
#  endif
# endif

// MeshGui
# ifndef MeshGuiExport
#  ifdef MeshGui_EXPORTS
#   define MeshGuiExport FREECAD_DECL_EXPORT
#  else
#   define MeshGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // MESH_GLOBAL_H
