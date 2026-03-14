#ifndef EXPRESSIONBINDINGPY_H
#define EXPRESSIONBINDINGPY_H

#include <CXX/Extensions.hxx>

class QWidget;
namespace Gui
{
class ExpressionBinding;

class ExpressionBindingPy: public Py::PythonClass<ExpressionBindingPy>
{
public:
    static void init_type();  // announce properties and methods

    ExpressionBindingPy(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);
    ~ExpressionBindingPy() override;

    Py::Object repr() override;

    Py::Object bind(const Py::Tuple&);
    Py::Object isBound();
    Py::Object apply(const Py::Tuple&);
    Py::Object hasExpression();
    Py::Object autoApply();
    Py::Object setAutoApply(const Py::Tuple&);

private:
    static ExpressionBinding* asBinding(QWidget*);

private:
    ExpressionBinding* expr;
};

}  // namespace Gui

#endif  // EXPRESSIONBINDING_H
