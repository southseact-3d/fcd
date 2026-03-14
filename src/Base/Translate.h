#ifndef BASE_TRANSLATE_H
#define BASE_TRANSLATE_H

#include <CXX/Extensions.hxx>
#include <QTranslator>
#include <memory>
#include <list>
#ifndef FC_GLOBAL_H
# include <FCGlobal.h>
#endif

namespace Py
{
class Object;
class Tuple;
}  // namespace Py

namespace Base
{

class BaseExport Translate: public Py::ExtensionModule<Translate>  // NOLINT
{
public:
    Translate();
    ~Translate() override;

private:
    Py::Object translate(const Py::Tuple& args);
    Py::Object translateNoop(const Py::Tuple& args);
    Py::Object translateNoop3(const Py::Tuple& args);
    Py::Object trNoop(const Py::Tuple& args);
    Py::Object installTranslator(const Py::Tuple& args);
    Py::Object removeTranslators(const Py::Tuple& args);

private:
    std::list<std::shared_ptr<QTranslator>> translators;
};

}  // namespace Base

#endif  // BASE_TRANSLATE_H
