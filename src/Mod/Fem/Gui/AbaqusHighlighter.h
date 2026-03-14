#ifndef FEMGUI_ABAQUSHIGHLIGHTER_H
#define FEMGUI_ABAQUSHIGHLIGHTER_H

#include <Gui/SyntaxHighlighter.h>

namespace FemGui
{

/**
 * Syntax highlighter for Abaqus.
 * @author Werner Mayer
 */
class AbaqusHighlighter: public Gui::SyntaxHighlighter
{
public:
    explicit AbaqusHighlighter(QObject* parent);
    ~AbaqusHighlighter() override;

protected:
    void highlightBlock(const QString& text) override;
};

}  // namespace FemGui

#endif  // FEMGUI_ABAQUSHIGHLIGHTER_H
