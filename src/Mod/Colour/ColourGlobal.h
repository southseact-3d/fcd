// SPDX-License-Identifier: LGPL-2.1-or-later

#include <FCGlobal.h>

#ifndef COLOUR_GLOBAL_H
# define COLOUR_GLOBAL_H


// Colour
# ifndef ColourExport
# ifdef Colour_EXPORTS
# define ColourExport FREECAD_DECL_EXPORT
# else
# define ColourExport FREECAD_DECL_IMPORT
# endif
# endif

// ColourGui
# ifndef ColourGuiExport
# ifdef ColourGui_EXPORTS
# define ColourGuiExport FREECAD_DECL_EXPORT
# else
# define ColourGuiExport FREECAD_DECL_IMPORT
# endif
# endif

#endif // COLOUR_GLOBAL_H
