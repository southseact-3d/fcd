#ifndef RENAMEOBJECTIDENTIFIEREXPRESSIONVISITOR_H
#define RENAMEOBJECTIDENTIFIEREXPRESSIONVISITOR_H

#include "Expression.h"

#include "Document.h"

namespace App
{

/**
 * @brief The RenameObjectIdentifierExpressionVisitor class is a functor used to visit each node of
 * an expression, and possibly rename VariableExpression nodes.
 */

template<class P>
class RenameObjectIdentifierExpressionVisitor: public ExpressionModifier<P>
{
public:
    RenameObjectIdentifierExpressionVisitor(
        P& _prop,
        const std::map<ObjectIdentifier, ObjectIdentifier>& _paths,
        const ObjectIdentifier& _owner)
        : ExpressionModifier<P>(_prop)
        , paths(_paths)
        , owner(_owner)
    {}

    void visit(Expression& node) override
    {
        this->renameObjectIdentifier(node, paths, owner);
    }

private:
    const std::map<ObjectIdentifier, ObjectIdentifier>&
        paths;                    /**< Map with current and new object identifiers */
    const ObjectIdentifier owner; /**< Owner of expression */
};

template<class P>
class UpdateElementReferenceExpressionVisitor: public ExpressionModifier<P>
{
public:
    explicit UpdateElementReferenceExpressionVisitor(P& _prop,
                                                     App::DocumentObject* feature = nullptr,
                                                     bool reverse = false)
        : ExpressionModifier<P>(_prop)
        , feature(feature)
        , reverse(reverse)
    {}

    void visit(Expression& node) override
    {
        this->updateElementReference(node, feature, reverse);
    }

private:
    App::DocumentObject* feature;
    bool reverse;
};

// Document relabel is not undoable, so we don't derive from
// ExpressionModifier, hence not calling aboutToSetValue/hasSetValue().
// By right, modification of document label should not change evaluation result
// of any expression.
class RelabelDocumentExpressionVisitor: public ExpressionVisitor
{
public:
    explicit RelabelDocumentExpressionVisitor(const App::Document& doc)
        : doc(doc)
    {}

    void visit(Expression& node) override
    {
        this->relabeledDocument(node, doc.getOldLabel(), doc.Label.getStrValue());
    }

private:
    const App::Document& doc;
};

template<class P>
class MoveCellsExpressionVisitor: public ExpressionModifier<P>
{
public:
    MoveCellsExpressionVisitor(P& prop, const CellAddress& address, int rowCount, int colCount)
        : ExpressionModifier<P>(prop)
        , address(address)
        , rowCount(rowCount)
        , colCount(colCount)
    {}

    void visit(Expression& node) override
    {
        this->moveCells(node, address, rowCount, colCount);
    }

private:
    CellAddress address;
    int rowCount;
    int colCount;
};

template<class P>
class OffsetCellsExpressionVisitor: public ExpressionModifier<P>
{
public:
    OffsetCellsExpressionVisitor(P& prop, int rowOffset, int colOffset)
        : ExpressionModifier<P>(prop)
        , rowOffset(rowOffset)
        , colOffset(colOffset)
    {}

    void visit(Expression& node) override
    {
        this->offsetCells(node, rowOffset, colOffset);
    }

private:
    int rowOffset;
    int colOffset;
};

}  // namespace App

#endif  // RENAMEOBJECTIDENTIFIEREXPRESSIONVISITOR_H
