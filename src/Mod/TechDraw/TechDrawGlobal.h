#include <FCGlobal.h>

#ifndef TECHDRAW_GLOBAL_H
#define TECHDRAW_GLOBAL_H


// TechDraw
#ifndef TechDrawExport
#ifdef TechDraw_EXPORTS
#  define TechDrawExport      FREECAD_DECL_EXPORT
#else
#  define TechDrawExport      FREECAD_DECL_IMPORT
#endif
#endif

// TechDrawGui
#ifndef TechDrawGuiExport
#ifdef TechDrawGui_EXPORTS
#  define TechDrawGuiExport   FREECAD_DECL_EXPORT
#else
#  define TechDrawGuiExport   FREECAD_DECL_IMPORT
#endif
#endif

#endif //TECHDRAW_GLOBAL_H
