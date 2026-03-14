#ifndef BASE_UNITSAPI_H
#define BASE_UNITSAPI_H

#include "UnitsSchema.h"
#include "UnitsSchemas.h"
#include "UnitsSchemasData.h"
#include "Quantity.h"


class QString;

using PyObject = struct _object;
using PyMethodDef = struct PyMethodDef;

namespace Base
{


class BaseExport UnitsApi
{
public:
    static std::unique_ptr<UnitsSchema> createSchema(std::size_t num);
    static void setSchema(const std::string& name);
    static void setSchema(std::size_t num);

    static std::string schemaTranslate(const Quantity& quant, double& factor, std::string& unitString);

    static std::string schemaTranslate(const Quantity& quant);

    static double toDouble(PyObject* args, const Base::Unit& u = Base::Unit());

    static void setDecimals(int);
    static int getDecimals();

    static void setDenominator(int);
    static int getDenominator();

    static std::vector<std::string> getDescriptions();
    static std::vector<std::string> getNames();

    static std::size_t count();

    static bool isMultiUnitAngle();
    static bool isMultiUnitLength();
    static std::string getBasicLengthUnit();

    static std::size_t getDefSchemaNum()
    {
        return schemas->spec().num;
    }
    // Python interface
    static PyMethodDef Methods[];

protected:
    static inline auto schemas = std::make_unique<UnitsSchemas>(UnitsSchemasData::unitSchemasDataPack);
    static inline int decimals {-1};
    static inline int denominator {-1};

    // the python API wrapper methods
    static PyObject* sParseQuantity(PyObject* self, PyObject* args);
    static PyObject* sListSchemas(PyObject* self, PyObject* args);
    static PyObject* sGetSchema(PyObject* self, PyObject* args);
    static PyObject* sSetSchema(PyObject* self, PyObject* args);
    static PyObject* sSchemaTranslate(PyObject* self, PyObject* args);
    static PyObject* sToNumber(PyObject* self, PyObject* args);
};

}  // namespace Base

#endif  // BASE_UNITSAPI_H
