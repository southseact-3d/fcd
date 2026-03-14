#include <QWidget>
#include <array>

class QBoxLayout;
class QLabel;
class QToolButton;

namespace StartGui
{

enum class Theme
{
    Classic,
    Light,
    Dark
};

/// A widget to allow selection of the UI theme (color scheme).
class ThemeSelectorWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ThemeSelectorWidget(QWidget* parent = nullptr);
    bool eventFilter(QObject* object, QEvent* event) override;

protected:
    void themeChanged(Theme newTheme);

private:
    void retranslateUi();
    void setupUi();
    void setupButtons(QBoxLayout* layout);
    void onLinkActivated(const QString& link);
    void preselectThemeFromSystemSettings();

    QLabel* _titleLabel;
    QLabel* _descriptionLabel;
    std::array<QToolButton*, 3> _buttons;
};

}  // namespace StartGui

#endif  // FREECAD_START_THEMESELECTORWIDGET_H
