#include <Base/PyWrapParseTupleAndKeywords.h>

#include "PartFeature.h"

// inclusion of the generated files (generated out of PartFeaturePy.xml)
#include "PartFeaturePy.h"
#include "PartFeaturePy.cpp"


using namespace Part;

// returns a string which represent the object e.g. when printed in python
std::string PartFeaturePy::representation() const
{
    return fmt::format("<{}>", static_cast<std::string>(getTypeId()));
}

PyObject* PartFeaturePy::getElementHistory(PyObject* args, PyObject* kwds) const
{
    const char* name;
    PyObject* recursive = Py_True;
    PyObject* sameType = Py_False;
    PyObject* showName = Py_False;

    static const std::array<const char*, 5>
        kwlist {"elementName", "recursive", "sameType", "showName", nullptr};
    if (!Base::Wrapped_ParseTupleAndKeywords(
            args,
            kwds,
            "s|OOO",
            kwlist,
            &name,
            &recursive,
            &sameType,
            &showName
        )) {
        return {};
    }

    auto feature = getFeaturePtr();
    Py::List list;
    bool showObjName = PyObject_IsTrue(showName);
    PY_TRY
    {
        std::string tmp;
        for (auto& history : Feature::getElementHistory(
                 feature,
                 name,
                 PyObject_IsTrue(recursive),
                 PyObject_IsTrue(sameType)
             )) {
            Py::Tuple ret(3);
            if (history.obj) {
                if (showObjName) {
                    ret.setItem(
                        0,
                        Py::TupleN(
                            Py::String(history.obj->getFullName()),
                            Py::String(history.obj->Label.getValue())
                        )
                    );
                }
                else {
                    ret.setItem(0, Py::Object(history.obj->getPyObject(), true));
                }
            }
            else {
                ret.setItem(0, Py::Long(history.tag));
            }
            tmp.clear();
            ret.setItem(1, Py::String(history.element.appendToBuffer(tmp)));
            Py::List intermedates;
            for (auto& h : history.intermediates) {
                tmp.clear();
                intermedates.append(Py::String(h.appendToBuffer(tmp)));
            }
            ret.setItem(2, intermedates);
            list.append(ret);
        }
        return Py::new_reference_to(list);
    }
    PY_CATCH;
}

PyObject* PartFeaturePy::getCustomAttributes(const char*) const
{
    return nullptr;
}

int PartFeaturePy::setCustomAttributes(const char*, PyObject*)
{
    return 0;
}
