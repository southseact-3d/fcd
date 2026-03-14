#ifndef MATERIAL_PYVARIANTS_H
#define MATERIAL_PYVARIANTS_H

#include <QMetaType>

#include <Base/Quantity.h>
#include <Base/QuantityPy.h>
#include <CXX/Objects.hxx>
#include <Gui/MetaTypes.h>

#include <Mod/Material/MaterialGlobal.h>

namespace Materials
{

extern MaterialsExport PyObject* _pyObjectFromVariant(const QVariant& value);
extern MaterialsExport Py::List getList(const QVariant& value);

}  // namespace Materials

#endif  // MATERIAL_PYVARIANTS_H
