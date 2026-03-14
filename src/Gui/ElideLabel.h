// This custom widget adds the missing ellipsize functionality in QT5

#ifndef ELIDELABEL_H
#define ELIDELABEL_H

#include <QLabel>
#include <QPainter>
#include <QFontMetrics>

#include <FCGlobal.h>

namespace Gui
{

class GuiExport ElideLabel: public QLabel
{
    Q_OBJECT

public:
    explicit ElideLabel(QWidget* parent = nullptr);
    ~ElideLabel() override = default;

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
};

}  // namespace Gui

#endif  // ELIDELABEL_H
