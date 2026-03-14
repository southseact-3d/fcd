//based on a python widget from:
//https://github.com/tcalmant/demo-ipopo-qt/blob/master/pc/details/compass.py

#ifndef COMPASSDIALWIDGET_H
#define COMPASSDIALWIDGET_H

#include <QWidget>
#include <QSize>

namespace TechDrawGui {

class CompassDialWidget : public QWidget
{
    Q_OBJECT

public:
    CompassDialWidget(QWidget* parent = nullptr);
    ~CompassDialWidget() override = default;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    double angle() const { return m_angle; }
    void setAngle(double newAngle);
    void setSize(int newSize);

public Q_SLOTS:
    void slotChangeAngle(double angle) { setAngle(angle); }
    void resetAngle() { setAngle(0.0); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void drawWidget(QPainter& painter);
    void drawNeedle(QPainter& painter);
    void drawMarkings(QPainter& painter);
    void drawBackground(QPainter& painter);

private:
    QRect m_rect;
    double m_angle;
    double m_margin;
    double m_markInterval;
    int m_defaultSize;
    int m_defaultMargin;
    int m_designRadius;
    int m_designDiameter;
};

} //namespace TechDrawGui
#endif // COMPASSDIALWIDGET_H
