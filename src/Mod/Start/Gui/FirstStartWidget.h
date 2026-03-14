#include <QWidget>
#include <QGroupBox>

class QLabel;
class QPushButton;

namespace StartGui
{

class ThemeSelectorWidget;
class GeneralSettingsWidget;

class FirstStartWidget: public QGroupBox
{
    Q_OBJECT
public:
    explicit FirstStartWidget(QWidget* parent = nullptr);
    bool eventFilter(QObject* object, QEvent* event) override;
    Q_SIGNAL void dismissed();

private:
    void retranslateUi();
    void setupUi();

    ThemeSelectorWidget* _themeSelectorWidget;
    GeneralSettingsWidget* _generalSettingsWidget;

    QLabel* _welcomeLabel;
    QLabel* _descriptionLabel;
    QPushButton* _doneButton;
};

}  // namespace StartGui

#endif  // FREECAD_FIRSTSTARTWIDGET_H
