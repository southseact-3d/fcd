#ifndef APP_APPLICATIONPY_H
#define APP_APPLICATIONPY_H

#include <Base/PyObjectBase.h>

namespace App
{

/** The ApplicationPy class
 * This is the Python wrapper class of Application.
 * @author Jürgen Riegel, Werner Mayer
 */
class AppExport ApplicationPy
{
public:
    // clang-format off
    // static python wrapper of the exported functions
    static PyObject* sGetParam               (PyObject *self, PyObject *args);
    static PyObject* sSaveParameter          (PyObject *self, PyObject *args);
    static PyObject* sGetVersion             (PyObject *self, PyObject *args);
    static PyObject* sGetConfig              (PyObject *self, PyObject *args);
    static PyObject* sSetConfig              (PyObject *self, PyObject *args);
    static PyObject* sDumpConfig             (PyObject *self, PyObject *args);
    static PyObject* sAddImportType          (PyObject *self, PyObject *args);
    static PyObject* sChangeImportModule     (PyObject *self, PyObject *args);
    static PyObject* sGetImportType          (PyObject *self, PyObject *args);
    static PyObject* sAddExportType          (PyObject *self, PyObject *args);
    static PyObject* sChangeExportModule     (PyObject *self, PyObject *args);
    static PyObject* sGetExportType          (PyObject *self, PyObject *args);
    static PyObject* sGetResourcePath        (PyObject *self, PyObject *args);
    static PyObject* sGetLibraryPath         (PyObject *self, PyObject *args);
    static PyObject* sGetTempPath            (PyObject *self, PyObject *args);
    static PyObject* sGetUserCachePath       (PyObject *self, PyObject *args);
    static PyObject* sGetUserConfigPath      (PyObject *self, PyObject *args);
    static PyObject* sGetUserAppDataPath     (PyObject *self, PyObject *args);
    static PyObject* sGetUserMacroPath       (PyObject *self, PyObject *args);
    static PyObject* sGetHelpPath            (PyObject *self, PyObject *args);
    static PyObject* sGetHomePath            (PyObject *self, PyObject *args);

    static PyObject* sLoadFile               (PyObject *self,PyObject *args);
    static PyObject* sOpenDocument           (PyObject *self,PyObject *args, PyObject *kwd);
    static PyObject* sSaveDocument           (PyObject *self,PyObject *args);
    static PyObject* sSaveDocumentAs         (PyObject *self,PyObject *args);
    static PyObject* sNewDocument            (PyObject *self,PyObject *args, PyObject *kwd);
    static PyObject* sCloseDocument          (PyObject *self,PyObject *args);
    static PyObject* sActiveDocument         (PyObject *self,PyObject *args);
    static PyObject* sSetActiveDocument      (PyObject *self,PyObject *args);
    static PyObject* sGetDocument            (PyObject *self,PyObject *args);
    static PyObject* sListDocuments          (PyObject *self,PyObject *args);
    static PyObject* sAddDocObserver         (PyObject *self,PyObject *args);
    static PyObject* sRemoveDocObserver      (PyObject *self,PyObject *args);
    static PyObject *sIsRestoring            (PyObject *self,PyObject *args);

    static PyObject *sSetLogLevel            (PyObject *self,PyObject *args);
    static PyObject *sGetLogLevel            (PyObject *self,PyObject *args);

    static PyObject *sCheckLinkDepth         (PyObject *self,PyObject *args);
    static PyObject *sGetLinksTo             (PyObject *self,PyObject *args);

    static PyObject *sGetDependentObjects    (PyObject *self,PyObject *args);

    static PyObject *sSetActiveTransaction   (PyObject *self,PyObject *args);
    static PyObject *sGetActiveTransaction   (PyObject *self,PyObject *args);
    static PyObject *sCloseActiveTransaction (PyObject *self,PyObject *args);
    static PyObject *sCheckAbort             (PyObject *self,PyObject *args);
    static PyMethodDef    Methods[];
    // clang-format on
};

}

#endif  // APP_APPLICATIONPY_H
