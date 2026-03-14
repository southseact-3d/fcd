#ifndef _OCCError_h_
#define _OCCError_h_

#include <Standard_Failure.hxx>
#include <Standard_Version.hxx>

#include <Base/Exception.h>
#include <Mod/Part/PartGlobal.h>


namespace Part
{
PartExport extern PyObject* PartExceptionOCCError;
PartExport extern PyObject* PartExceptionOCCDomainError;
PartExport extern PyObject* PartExceptionOCCRangeError;
PartExport extern PyObject* PartExceptionOCCConstructionError;
PartExport extern PyObject* PartExceptionOCCDimensionError;


#define PY_TRY try

/// see docu of PY_TRY
#define _PY_CATCH_OCC(R) \
    catch (Standard_Failure & e) \
    { \
        std::string str; \
        Standard_CString msg = e.GetMessageString(); \
        str += typeid(e).name(); \
        str += " "; \
        if (msg) { \
            str += msg; \
        } \
        else { \
            str += "No OCCT Exception Message"; \
        } \
        _Py_Error(R, Part::PartExceptionOCCError, str.c_str()); \
    } \
    _PY_CATCH(R)
}  // namespace Part

#define PY_CATCH_OCC _PY_CATCH_OCC(return (NULL))
#endif  // _OCCError_h_
