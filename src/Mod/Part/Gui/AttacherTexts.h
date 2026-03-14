#ifndef PARTATTACHERTEXTS_H
#define PARTATTACHERTEXTS_H

#include <vector>
#include <QString>
#include <QStringList>
#include <Mod/Part/App/Attacher.h>

#include <Mod/Part/PartGlobal.h>

namespace AttacherGui
{

using TextSet = std::vector<QString>;

/**
 * @brief getUIStrings
 * @param attacherType
 * @param mmode
 * @return vector of two QStrings:
 * first is the name of attachment mode. e.g. "Tangent to surface";
 * second is tooltip-style explanation of the mode, like "Plane is tangent to a surface at vertex."
 */
TextSet PartGuiExport getUIStrings(Base::Type attacherType, Attacher::eMapMode mmode);


QString PartGuiExport getShapeTypeText(Attacher::eRefType type);

QStringList PartGuiExport getRefListForMode(Attacher::AttachEngine& attacher, Attacher::eMapMode mmode);


// Python interface
class PartGuiExport AttacherGuiPy
{
public:
    static PyMethodDef Methods[];
    static PyObject* sGetModeStrings(PyObject* /*self*/, PyObject* args);
    static PyObject* sGetRefTypeUserFriendlyName(PyObject* /*self*/, PyObject* args);
};

}  // namespace AttacherGui
#endif
