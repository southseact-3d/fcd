#ifndef DAGRECTITEM_H
#define DAGRECTITEM_H

#include <QBrush>
#include <QGraphicsRectItem>


namespace Gui
{
namespace DAG
{
/*all right I give up! the parenting combined with the zvalues is fubar!
 * you can't control any kind of layering between children of separate parents
 */
class RectItem: public QGraphicsRectItem
{
public:
    explicit RectItem(QGraphicsItem* parent = nullptr);
    void setBackgroundBrush(const QBrush& brushIn)
    {
        backgroundBrush = brushIn;
    }
    void setEditingBrush(const QBrush& brushIn)
    {
        editBrush = brushIn;
    }
    void preHighlightOn()
    {
        preSelected = true;
    }
    void preHighlightOff()
    {
        preSelected = false;
    }
    void selectionOn()
    {
        selected = true;
    }
    void selectionOff()
    {
        selected = false;
    }
    bool isSelected()
    {
        return selected;
    }
    bool isPreSelected()
    {
        return preSelected;
    }
    void editingStart()
    {
        editing = true;
    }
    void editingFinished()
    {
        editing = false;
    }
    bool isEditing()
    {
        return editing;
    }

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QBrush backgroundBrush;  //!< brush used for background. not used yet.
    QBrush editBrush;        //!< brush used when object is in edit mode.
    // start with booleans, may expand to state.
    bool selected;
    bool preSelected;
    bool editing;
};
}  // namespace DAG
}  // namespace Gui

#endif  // DAGRECTITEM_H
