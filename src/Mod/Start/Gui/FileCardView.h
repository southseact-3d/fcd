#include <QListView>

namespace StartGui
{

class FileCardView: public QListView
{
    Q_OBJECT

public:
    explicit FileCardView(QWidget* parent = nullptr);

    int heightForWidth(int width) const override;

    QSize sizeHint() const override;

private:
    int m_cardSpacing;
};

}  // namespace StartGui

#endif  // FREECAD_START_FILECARDVIEW_H
