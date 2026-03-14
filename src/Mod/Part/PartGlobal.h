#include <FCGlobal.h>

#ifndef PART_GLOBAL_H
# define PART_GLOBAL_H

// Part
# ifndef PartExport
#  ifdef Part_EXPORTS
#   define PartExport FREECAD_DECL_EXPORT
#  else
#   define PartExport FREECAD_DECL_IMPORT
#  endif
# endif

# ifndef AttacherExport
#  define AttacherExport PartExport
# endif

// PartGui
# ifndef PartGuiExport
#  ifdef PartGui_EXPORTS
#   define PartGuiExport FREECAD_DECL_EXPORT
#  else
#   define PartGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // PART_GLOBAL_H
