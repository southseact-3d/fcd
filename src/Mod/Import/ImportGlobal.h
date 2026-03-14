#include <FCGlobal.h>

#ifndef IMPORT_GLOBAL_H
# define IMPORT_GLOBAL_H


// Import
# ifndef ImportExport
#  ifdef Import_EXPORTS
#   define ImportExport FREECAD_DECL_EXPORT
#  else
#   define ImportExport FREECAD_DECL_IMPORT
#  endif
# endif

// ImportGui
# ifndef ImportGuiExport
#  ifdef ImportGui_EXPORTS
#   define ImportGuiExport FREECAD_DECL_EXPORT
#  else
#   define ImportGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // IMPORT_GLOBAL_H
