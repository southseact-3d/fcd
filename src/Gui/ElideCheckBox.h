// This custom widget adds the missing ellipsize functionality in QT5

#ifndef ELIDECHECKBOX_H
#define ELIDECHECKBOX_H

#include <QCheckBox>
#include <QPainter>
#include <QFontMetrics>
#include <QStyleOptionButton>

#include <FCGlobal.h>

namespace Gui
{

class GuiExport ElideCheckBox: public QCheckBox
{
    Q_OBJECT

public:
    explicit ElideCheckBox(QWidget* parent = nullptr);
    ~ElideCheckBox() override = default;

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
};

}  // namespace Gui

#endif  // ELIDECHECKBOX_H
