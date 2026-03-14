#ifndef CLOUD_GLOBAL_H
#define CLOUD_GLOBAL_H

#include <FCGlobal.h>

// Cloud
#ifndef CloudAppExport
# ifdef CloudApp_EXPORTS
#  define CloudAppExport FREECAD_DECL_EXPORT
# else
#  define CloudAppExport FREECAD_DECL_IMPORT
# endif
#endif

// Cloud
#ifndef CloudGuiExport
# ifdef CloudGui_EXPORTS
#  define CloudGuiExport FREECAD_DECL_EXPORT
# else
#  define CloudGuiExport FREECAD_DECL_IMPORT
# endif
#endif


#endif  // CLOUD_GLOBAL_H
