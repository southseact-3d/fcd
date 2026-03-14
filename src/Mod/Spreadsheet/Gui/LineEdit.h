#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <Gui/ExpressionCompleter.h>
#include <QListView>


namespace SpreadsheetGui
{

class LineEdit: public Gui::ExpressionLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(QWidget* parent = nullptr);
    void setDocumentObject(const App::DocumentObject* currentDocObj, bool checkInList = true);

Q_SIGNALS:
    void finishedWithKey(int key, Qt::KeyboardModifiers modifiers);

private:
    int lastKeyPressed;
    Qt::KeyboardModifiers lastModifiers;

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};

/* QCompleter uses a parentless QListView as a popup, whose geometry
 * is corrected using its own algorithm, which does not take into account QGraphicsScene,
 * therefore we have to use our own widget to adjust the geometry. */
class XListView: public QListView
{
    Q_OBJECT
public:
    explicit XListView(LineEdit* parent);

Q_SIGNALS:
    void geometryChanged(void);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void updateGeometries(void) override;
};

}  // namespace SpreadsheetGui

#endif  // LINEEDIT_H
