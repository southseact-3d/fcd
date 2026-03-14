#include <FCGlobal.h>

#ifndef WEB_GLOBAL_H
# define WEB_GLOBAL_H


// Web
# ifndef WebExport
#  ifdef Web_EXPORTS
#   define WebExport FREECAD_DECL_EXPORT
#  else
#   define WebExport FREECAD_DECL_IMPORT
#  endif
# endif

// WebGui
# ifndef WebGuiExport
#  ifdef WebGui_EXPORTS
#   define WebGuiExport FREECAD_DECL_EXPORT
#  else
#   define WebGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // WEB_GLOBAL_H
