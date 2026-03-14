#include <FCGlobal.h>

#ifndef _TEMPLATE__GLOBAL_H
# define _TEMPLATE__GLOBAL_H


// _TEMPLATE_
# ifndef _TEMPLATE_Export
#  ifdef _TEMPLATE__EXPORTS
#   define _TEMPLATE_Export FREECAD_DECL_EXPORT
#  else
#   define _TEMPLATE_Export FREECAD_DECL_IMPORT
#  endif
# endif

// _TEMPLATE_Gui
# ifndef _TEMPLATE_GuiExport
#  ifdef _TEMPLATE_Gui_EXPORTS
#   define _TEMPLATE_GuiExport FREECAD_DECL_EXPORT
#  else
#   define _TEMPLATE_GuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  //_TEMPLATE__GLOBAL_H
