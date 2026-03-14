#ifndef GUI_TREEITEMMODE_H
#define GUI_TREEITEMMODE_H

namespace Gui
{

/// highlight modes for the tree items
enum class HighlightMode
{
    Underlined,
    Italic,
    Overlined,
    StrikeOut,
    Bold,
    Blue,
    LightBlue,
    UserDefined
};

/// highlight modes for the tree items
enum class TreeItemMode
{
    ExpandItem,
    ExpandPath,
    CollapseItem,
    ToggleItem
};
}  // namespace Gui

#endif  // GUI_TREEITEMMODE_H
