#ifndef QTCOLORPICKER_H
#define QTCOLORPICKER_H
#include <QPushButton>
#include <QtCore/QString>
#include <QtGui/QColor>

#include <QLabel>
#include <QtCore/QEvent>
#include <QtGui/QFocusEvent>

#if defined(Q_OS_WIN)
#  if !defined(QT_QTCOLORPICKER_EXPORT) && !defined(QT_QTCOLORPICKER_IMPORT)
#    define QT_QTCOLORPICKER_EXPORT
#  elif defined(QT_QTCOLORPICKER_IMPORT)
#    if defined(QT_QTCOLORPICKER_EXPORT)
#      undef QT_QTCOLORPICKER_EXPORT
#    endif
#    define QT_QTCOLORPICKER_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOLORPICKER_EXPORT)
#    undef QT_QTCOLORPICKER_EXPORT
#    define QT_QTCOLORPICKER_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOLORPICKER_EXPORT
#endif

class ColorPickerPopup;

class QT_QTCOLORPICKER_EXPORT QtColorPicker : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(bool colorDialog READ colorDialogEnabled WRITE setColorDialogEnabled) // clazy:exclude=qproperty-without-notify

public:
    QtColorPicker(QWidget *parent = nullptr,
                  int columns = -1, bool enableColorDialog = true);

    ~QtColorPicker() override;

    void insertColor(const QColor &color, const QString &text = QString(), int index = -1);

    QColor currentColor() const;

    QColor color(int index) const;

    void setColorDialogEnabled(bool enabled);
    bool colorDialogEnabled() const;

    void setStandardColors();

    static QColor getColor(const QPoint &pos, bool allowCustomColors = true);

public Q_SLOTS:
    void setCurrentColor(const QColor &col);

Q_SIGNALS:
    void colorChanged(const QColor &);
    void colorSet(const QColor &);

protected:
    void paintEvent(QPaintEvent *e) override;

private Q_SLOTS:
    void buttonPressed(bool toggled);
    void popupClosed();

private:
    ColorPickerPopup *popup;
    QColor col;
    bool withColorDialog;
    bool dirty;
    bool firstInserted;
};

#endif
// clang-format on
