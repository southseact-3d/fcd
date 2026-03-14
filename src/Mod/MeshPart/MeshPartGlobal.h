#include <FCGlobal.h>

#ifndef MESHPART_GLOBAL_H
# define MESHPART_GLOBAL_H


// MeshPart
# ifndef MeshPartExport
#  ifdef MeshPart_EXPORTS
#   define MeshPartExport FREECAD_DECL_EXPORT
#  else
#   define MeshPartExport FREECAD_DECL_IMPORT
#  endif
# endif

// MeshPartGui
# ifndef MeshPartGuiExport
#  ifdef MeshPartGui_EXPORTS
#   define MeshPartGuiExport FREECAD_DECL_EXPORT
#  else
#   define MeshPartGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // MESHPART_GLOBAL_H
