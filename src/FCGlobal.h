#ifndef FC_GLOBAL_H
#define FC_GLOBAL_H


#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(__CYGWIN__)
#  define FREECAD_DECL_EXPORT __declspec(dllexport)
#  define FREECAD_DECL_IMPORT __declspec(dllimport)
#else
#  define FREECAD_DECL_EXPORT
#  define FREECAD_DECL_IMPORT
#endif

// FreeCADBase
#ifdef FreeCADBase_EXPORTS
#  define BaseExport  FREECAD_DECL_EXPORT
#else
#  define BaseExport  FREECAD_DECL_IMPORT
#endif

// FreeCADApp
#ifdef FreeCADApp_EXPORTS
#       define AppExport   FREECAD_DECL_EXPORT
#       define DataExport  FREECAD_DECL_EXPORT
#else
#       define AppExport   FREECAD_DECL_IMPORT
#       define DataExport  FREECAD_DECL_IMPORT
#endif

// FreeCADGui
#ifdef FreeCADGui_EXPORTS
#  define GuiExport   FREECAD_DECL_EXPORT
#else
#  define GuiExport   FREECAD_DECL_IMPORT
#endif

// Disable copy/move
#define FC_DISABLE_COPY(Class) \
    Class(const Class &) = delete;\
    Class &operator=(const Class &) = delete;

#define FC_DISABLE_MOVE(Class) \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete;

#define FC_DISABLE_COPY_MOVE(Class) \
    FC_DISABLE_COPY(Class) \
    FC_DISABLE_MOVE(Class)

// Default copy/move
#define FC_DEFAULT_COPY(Class) \
    Class(const Class &) = default;\
    Class &operator=(const Class &) = default;

#define FC_DEFAULT_MOVE(Class) \
    Class(Class &&) = default; \
    Class &operator=(Class &&) = default;

#define FC_DEFAULT_COPY_MOVE(Class) \
    FC_DEFAULT_COPY(Class) \
    FC_DEFAULT_MOVE(Class)

#include <QtCore.h>
#ifndef HAVE_Q_DISABLE_COPY_MOVE
#define Q_DISABLE_COPY_MOVE FC_DEFAULT_COPY_MOVE
#endif

#endif //FC_GLOBAL_H
