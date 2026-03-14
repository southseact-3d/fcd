#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>

namespace Gui
{

class SplitButton: public QWidget
{
    Q_OBJECT
public:
    explicit SplitButton(QWidget* parent = nullptr);
    explicit SplitButton(const QString& text, QWidget* parent = nullptr);

    QPushButton* mainButton() const
    {
        return m_main;
    }

    QToolButton* menuButton() const
    {
        return m_menuButton;
    }

    QMenu* menu() const
    {
        return m_menu;
    }

Q_SIGNALS:
    void defaultClicked();
    void triggered(QAction*);

private:
    QPushButton* m_main;
    QToolButton* m_menuButton;
    QMenu* m_menu;
};

}  // namespace Gui

#endif  // FREECAD_FCSPLITBUTTON_H
