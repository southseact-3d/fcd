#ifndef COMPASSWIDGET_H
#define COMPASSWIDGET_H

#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QSize>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QHBoxLayout;
class QPushButton;
class QVBoxLayout;
QT_END_NAMESPACE

namespace Gui {
class QuantitySpinBox;
}

namespace TechDrawGui
{

class CompassDialWidget;

class CompassWidget: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double angle READ dialAngle WRITE setDialAngle NOTIFY angleChanged)

public:
    CompassWidget(QWidget* parent = nullptr);
    ~CompassWidget() override = default;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool eventFilter(QObject* target, QEvent* event) override;
    void retranslateUi();
    double dialAngle() const { return m_angle; }
    double value() const { return m_angle; }
    double positiveValue() { return m_angle < 0.0 ? m_angle + 360.0 : m_angle; }
    void setDialAngle(double newAngle);
    void setAdvanceIncrement(double newIncrement);
    double advanceIncrement() const { return m_advanceIncrement; }

Q_SIGNALS:
    void angleChanged(double angle);
    void angleSet(double angle);

public Q_SLOTS:
    void slotChangeAngle(double angle) { setDialAngle(angle); }
    void slotSpinBoxEnter(double newAngle);
    void resetAngle() { setDialAngle(0.0); }//conventional angles
    void setToEast() { setDialAngle(0.0); }
    void setToNorth() { setDialAngle(90.0); }
    void setToWest() { setDialAngle(180.0); }
    void setToSouth() { setDialAngle(270.0); }
    void slotCWAdvance();
    void slotCCWAdvance();

protected:
    void paintEvent(QPaintEvent* event) override;
    void buildWidget();
    double changeAngleConvention(double CWY) const;

private:
    QRect m_rect;
    int m_minimumWidth;
    int m_minimumHeight;
    int m_defaultMargin;
    double m_angle;
    double m_advanceIncrement;

    QVBoxLayout* compassLayout;
    QHBoxLayout* compassDialLayout;
    QHBoxLayout* compassControlLayout;

    CompassDialWidget* compassDial;
    //    DoubleSpinBoxNoEnter* dsbAngle;
    Gui::QuantitySpinBox* dsbAngle;
    QLabel* compassControlLabel;
    QPushButton* pbCWAdvance;
    QPushButton* pbCCWAdvance;
};

}//namespace TechDrawGui
#endif// COMPASSWIDGET_H
