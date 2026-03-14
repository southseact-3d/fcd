#ifndef SPREADSHEET_GLOBAL_H
#define SPREADSHEET_GLOBAL_H

#include <FCGlobal.h>


// Spreadsheet
#ifndef SpreadsheetExport
# ifdef Spreadsheet_EXPORTS
#  define SpreadsheetExport FREECAD_DECL_EXPORT
# else
#  define SpreadsheetExport FREECAD_DECL_IMPORT
# endif
#endif

// SpreadsheetGui
#ifndef SpreadsheetGuiExport
# ifdef SpreadsheetGui_EXPORTS
#  define SpreadsheetGuiExport FREECAD_DECL_EXPORT
# else
#  define SpreadsheetGuiExport FREECAD_DECL_IMPORT
# endif
#endif

#endif  // SPREADSHEET_GLOBAL_H
