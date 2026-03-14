#include <FCGlobal.h>

#ifndef ASSEMBLY_GLOBAL_H
# define ASSEMBLY_GLOBAL_H


// Assembly
# ifndef AssemblyExport
#  ifdef Assembly_EXPORTS
#   define AssemblyExport FREECAD_DECL_EXPORT
#  else
#   define AssemblyExport FREECAD_DECL_IMPORT
#  endif
# endif

// AssemblyGui
# ifndef AssemblyGuiExport
#  ifdef AssemblyGui_EXPORTS
#   define AssemblyGuiExport FREECAD_DECL_EXPORT
#  else
#   define AssemblyGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // ASSEMBLY_GLOBAL_H
