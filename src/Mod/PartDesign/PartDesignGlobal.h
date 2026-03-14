#ifndef PARTDESIGN_GLOBAL_H
#define PARTDESIGN_GLOBAL_H

#include <FCGlobal.h>


// PartDesign
#ifndef PartDesignExport
# ifdef PartDesign_EXPORTS
#  define PartDesignExport FREECAD_DECL_EXPORT
# else
#  define PartDesignExport FREECAD_DECL_IMPORT
# endif
#endif

// PartDesignGui
#ifndef PartDesignGuiExport
# ifdef PartDesignGui_EXPORTS
#  define PartDesignGuiExport FREECAD_DECL_EXPORT
# else
#  define PartDesignGuiExport FREECAD_DECL_IMPORT
# endif
#endif

#endif  // PARTDESIGN_GLOBAL_H
