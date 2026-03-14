#ifndef VECTOREDITWIDGET_H
#define VECTOREDITWIDGET_H

#include <QWidget>
#include <QSize>
#include <QString>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QToolButton;
class QSpacerItem;
QT_END_NAMESPACE

namespace Gui {
class DoubleSpinBox;
}

#include <Base/Vector3D.h>

namespace TechDrawGui {

class VectorEditWidget : public QWidget
{
    Q_OBJECT

public:
    VectorEditWidget(QWidget* parent = 0);
    ~VectorEditWidget() override = default;

    QSize minimumSizeHint() const override;
    bool eventFilter(QObject *target, QEvent *event) override;

    void setLabel(std::string newLabel);
    void setLabel(QString newLabel);
    Base::Vector3d value() const { return m_value; }

Q_SIGNALS:
    void valueChanged(Base::Vector3d newValue);

public Q_SLOTS:
    void setValue(Base::Vector3d newValue);
    void setValueNoNotify(Base::Vector3d newValue);

protected:
    void buildWidget();

protected Q_SLOTS:
    void slotExpandButtonToggled(bool checked);
    void slotXValueChanged(double newValue);
    void slotYValueChanged(double newValue);
    void slotZValueChanged(double newValue);

    void updateDisplay();

private:
    int m_minimumWidth;
    int m_minimumHeight;
    int m_expandedHeight;
    bool m_blockNotify;

    QSize m_size;

    Base::Vector3d m_value;

    QVBoxLayout *vectorEditLayout;
    QHBoxLayout *VectorEditButtonLayout;
    QLabel *lvectorName;
    QLineEdit *leVectorDisplay;
    QToolButton *tbExpand;
    QGridLayout *VectorEditItemLayout;
    Gui::DoubleSpinBox *dsbX;
    Gui::DoubleSpinBox *dsbY;
    Gui::DoubleSpinBox *dsbZ;
    QLabel *lX;
    QLabel *lY;
    QLabel *lZ;
    QSpacerItem *verticalSpacer;
};

} //namespace TechDrawGui
#endif // VECTOREDITWIDGET_H
