#include <FCGlobal.h>

#ifndef INSPECTION_GLOBAL_H
# define INSPECTION_GLOBAL_H


// Inspection
# ifndef InspectionExport
#  ifdef Inspection_EXPORTS
#   define InspectionExport FREECAD_DECL_EXPORT
#  else
#   define InspectionExport FREECAD_DECL_IMPORT
#  endif
# endif

// InspectionGui
# ifndef InspectionGuiExport
#  ifdef InspectionGui_EXPORTS
#   define InspectionGuiExport FREECAD_DECL_EXPORT
#  else
#   define InspectionGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // INSPECTION_GLOBAL_H
