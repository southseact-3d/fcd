#ifndef QUANTITYSPINBOX_P_H
#define QUANTITYSPINBOX_P_H

#include <QLabel>
#include <QMouseEvent>
#include <QLineEdit>

class ExpressionLabel: public QLabel
{
    Q_OBJECT
public:
    ExpressionLabel(QWidget* parent)
        : QLabel(parent)
    {}

    void setExpressionText(const QString& text)
    {
        if (text.isEmpty()) {
            this->setToolTip(genericExpressionEditorTooltip);
        }
        else {
            this->setToolTip(expressionEditorTooltipPrefix + text);
        }
    }

    void show()
    {
        if (auto parentLineEdit = qobject_cast<QLineEdit*>(parent())) {
            // horizontal margin, so text will not be behind the icon
            QMargins margins = parentLineEdit->contentsMargins();
            margins.setRight(2 * margins.right() + sizeHint().width());
            parentLineEdit->setContentsMargins(margins);
        }
        QLabel::show();
    }

protected:
    void mouseReleaseEvent(QMouseEvent* event) override
    {
        if (rect().contains(event->pos())) {
            Q_EMIT clicked();
        }
    }

Q_SIGNALS:
    void clicked();

private:
    const QString genericExpressionEditorTooltip = tr("Enter expression… (=)");
    const QString expressionEditorTooltipPrefix = tr("Expression:") + QStringLiteral(" ");
};

#endif  // QUANTITYSPINBOX_P_H
