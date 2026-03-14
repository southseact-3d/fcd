#ifndef STYLEPARAMETERS_PARSER_H
#define STYLEPARAMETERS_PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "ParameterManager.h"

namespace Gui::StyleParameters
{

enum class Operator : std::uint8_t
{
    Add,
    Subtract,
    Multiply,
    Divide
};

struct EvaluationContext
{
    const ParameterManager* manager {};
    ParameterManager::ResolveContext context;
};

// Abstract Syntax Tree (AST) Base
struct GuiExport Expr
{
    Expr() = default;

    FC_DEFAULT_MOVE(Expr);
    FC_DISABLE_COPY(Expr);

    virtual Value evaluate(const EvaluationContext& context) const = 0;
    virtual ~Expr() = default;
};

struct GuiExport ParameterReference: public Expr
{
    std::string name;

    explicit ParameterReference(std::string name)
        : name(std::move(name))
    {}

    Value evaluate(const EvaluationContext& context) const override;
};

struct GuiExport Number: public Expr
{
    Numeric value;

    Number(double value, std::string unit)
        : value({value, std::move(unit)})
    {}

    Value evaluate([[maybe_unused]] const EvaluationContext& context) const override;
};

struct GuiExport Color: public Expr
{
    Base::Color color;

    explicit Color(Base::Color color)
        : color(std::move(color))
    {}

    Value evaluate([[maybe_unused]] const EvaluationContext& context) const override;
};

struct GuiExport FunctionCall: public Expr
{
    std::string functionName;
    std::vector<std::unique_ptr<Expr>> arguments;

    FunctionCall(std::string functionName, std::vector<std::unique_ptr<Expr>> arguments)
        : functionName(std::move(functionName))
        , arguments(std::move(arguments))
    {}

    Value evaluate(const EvaluationContext& context) const override;
};

struct GuiExport BinaryOp: public Expr
{
    std::unique_ptr<Expr> left, right;
    Operator op;

    BinaryOp(std::unique_ptr<Expr> left, Operator op, std::unique_ptr<Expr> right)
        : left(std::move(left))
        , right(std::move(right))
        , op(op)
    {}

    Value evaluate(const EvaluationContext& context) const override;
};

struct GuiExport UnaryOp: public Expr
{
    Operator op;
    std::unique_ptr<Expr> operand;

    UnaryOp(Operator op, std::unique_ptr<Expr> operand)
        : op(op)
        , operand(std::move(operand))
    {}

    Value evaluate(const EvaluationContext& context) const override;
};

class GuiExport Parser
{
    static constexpr auto rgbFunction = "rgb(";
    static constexpr auto rgbaFunction = "rgba(";

    std::string input;
    size_t pos = 0;

public:
    explicit Parser(std::string input)
        : input(std::move(input))
    {}

    std::unique_ptr<Expr> parse();

private:
    bool peekString(const char* function) const;
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    bool peekColor();
    std::unique_ptr<Expr> parseColor();
    bool peekParameter();
    std::unique_ptr<Expr> parseParameter();
    bool peekFunction();
    std::unique_ptr<Expr> parseFunctionCall();
    int parseInt();
    std::unique_ptr<Expr> parseNumber();
    std::string parseUnit();
    bool match(char expected);
    void skipWhitespace();
};

}  // namespace Gui::StyleParameters

#endif  // STYLEPARAMETERS_PARSER_H
