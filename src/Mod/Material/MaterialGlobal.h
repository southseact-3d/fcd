#include <FCGlobal.h>

#ifndef MATERIAL_GLOBAL_H
#define MATERIAL_GLOBAL_H

// Material
#ifndef MaterialsExport
#ifdef Materials_EXPORTS
#define MaterialsExport FREECAD_DECL_EXPORT
#else
#define MaterialsExport FREECAD_DECL_IMPORT
#endif
#endif

// MatGui
#ifndef MatGuiExport
#ifdef MatGui_EXPORTS
#define MatGuiExport FREECAD_DECL_EXPORT
#else
#define MatGuiExport FREECAD_DECL_IMPORT
#endif
#endif

#endif  // MATERIAL_GLOBAL_H
