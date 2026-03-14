#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <App/Application.h>

namespace StartGui
{

struct NewButton
{
    QString heading;
    QString description;
    QString iconPath;
};

class NewFileButton: public QPushButton
{
public:
    explicit NewFileButton(const NewButton& newButton);

private:
    int iconSize;
    int labelWidth;
    QHBoxLayout* mainLayout;
    QVBoxLayout* textLayout;
    QLabel* headingLabel;

protected:
    QSize minimumSizeHint() const override;
};

}  // namespace StartGui

#endif  // FREECAD_NEWFILEBUTTON_H
