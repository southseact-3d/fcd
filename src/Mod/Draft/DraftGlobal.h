#include <FCGlobal.h>

#ifndef DRAFT_GLOBAL_H
# define DRAFT_GLOBAL_H


// Draft
# ifndef DraftUtilsExport
#  ifdef DraftUtils_EXPORTS
#   define DraftUtilsExport FREECAD_DECL_EXPORT
#  else
#   define DraftUtilsExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // DRAFT_GLOBAL_H
