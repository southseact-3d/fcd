#ifndef PPEMBED_H
#define PPEMBED_H

#ifdef __cplusplus
extern "C" {             /* a C library, but callable from C++ */
#endif

#include <stdio.h> //NOLINT
// Python
#if defined (_POSIX_C_SOURCE)
#	undef  _POSIX_C_SOURCE
#endif // (re-)defined in pyconfig.h
#include <Python.h>
#ifndef FC_GLOBAL_H
#include <FCGlobal.h>
#endif

extern int PP_RELOAD;    /* 1=reload py modules when attributes referenced */
extern int PP_DEBUG;     /* 1=start debugger when string/function/member run */

typedef enum {           //NOLINT
     PP_EXPRESSION,      /* which kind of code-string */
     PP_STATEMENT        /* expressions and statements differ */
} PPStringModes;


/***************************************************/
/*  ppembed-modules.c: load,access module objects  */
/***************************************************/

extern const char *PP_Init(const char *modname);
extern int         PP_Make_Dummy_Module(const char *modname);
extern PyObject   *PP_Load_Module(const char *modname);
extern PyObject   *PP_Load_Attribute(const char *modname, const char *attrname);
extern int         PP_Run_Command_Line(const char *prompt);


/**********************************************************/
/*  ppembed-globals.c: read,write module-level variables  */
/**********************************************************/

extern int
    PP_Convert_Result(PyObject *presult, const char *resFormat, void *resTarget);

extern int
    PP_Get_Global(const char *modname, const char *varname, const char *resfmt, void *cresult);

extern int
    PP_Set_Global(const char *modname, const char *varname, const char *valfmt, ... /*val*/);


/***************************************************/
/*  ppembed-strings.c: run strings of Python code  */
/***************************************************/

extern int                                         /* run C string of code */
    PP_Run_Codestr(PPStringModes mode,             /* code=expr or stmt?  */
                   const char *code,   const char *modname,    /* codestr, modnamespace */
                   const char *resfmt, void *cresult);   /* result type, target */

extern PyObject*
    PP_Debug_Codestr(PPStringModes mode,           /* run string in pdb */
                     const char *codestring, PyObject *moddict);

extern PyObject *
    PP_Compile_Codestr(PPStringModes mode,
                       const char *codestr);             /* precompile to bytecode */

extern int
    PP_Run_Bytecode(PyObject *codeobj,             /* run a bytecode object */
                    const char     *modname,
                    const char     *resfmt, void *restarget);

extern PyObject *                                  /* run bytecode under pdb */
    PP_Debug_Bytecode(PyObject *codeobject, PyObject *moddict);


/*******************************************************/
/*  ppembed-callables.c: call functions, classes, etc. */
/*******************************************************/

extern BaseExport int                                       /* mod.func(args) */
    PP_Run_Function(const char *modname, const char *funcname,          /* func|classname */
                    const char *resfmt,  void *cresult,           /* result target  */
                    const char *argfmt,  ... /* arg, arg... */ ); /* input arguments*/

extern PyObject*
    PP_Debug_Function(PyObject *func, PyObject *args);   /* call func in pdb */

extern int
    PP_Run_Known_Callable(PyObject *object,              /* func|class|method */
                          const char *resfmt, void *restarget, /* skip module fetch */
                          const char *argfmt, ... /* arg,.. */ );


/**************************************************************/
/*  ppembed-attributes.c: run object methods, access members  */
/**************************************************************/

extern int
    PP_Run_Method(PyObject *pobject, const char *method,     /* uses Debug_Function */
                      const char *resfmt,  void *cresult,              /* output */
                      const char *argfmt,  ... /* arg, arg... */ );    /* inputs */

extern int
    PP_Get_Member(PyObject *pobject, const char *attrname,
                      const char *resfmt,  void *cresult);             /* output */

extern int
    PP_Set_Member(PyObject *pobject, const char *attrname,
                      const char *valfmt,  ... /* val, val... */ );    /* input */


/**********************************************************/
/*  ppembed-errors.c: get exception data after api error  */
/**********************************************************/

extern void PP_Fetch_Error_Text();    /* fetch (and clear) exception */

extern char PP_last_error_type[];     /* exception name text */
extern char PP_last_error_info[];     /* exception data text */
extern char PP_last_error_trace[];    /* exception traceback text */

extern PyObject *PP_PyDict_Object;    /* saved PyDict object */
extern PyObject *PP_last_traceback;   /* saved exception traceback object */
extern PyObject *PP_last_exception_type;   /* saved exception type */


#ifdef __cplusplus
}
#endif

#endif /*PREEMBED_H*/
// clang-format on
