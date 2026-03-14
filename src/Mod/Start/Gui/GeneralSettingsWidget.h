#include <QWidget>
#include <gsl/pointers>

class QLabel;
class QComboBox;

namespace StartGui
{

class GeneralSettingsWidget: public QWidget
{
    Q_OBJECT
public:
    explicit GeneralSettingsWidget(QWidget* parent = nullptr);

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    void retranslateUi();

    void setupUi();
    void createHorizontalUi();

    QString createLabelText(const QString& translatedText) const;
    gsl::owner<QComboBox*> createLanguageComboBox();
    gsl::owner<QComboBox*> createUnitSystemComboBox();
    gsl::owner<QComboBox*> createNavigationStyleComboBox();

    void onLanguageChanged(int index);
    void onUnitSystemChanged(int index);
    void onNavigationStyleChanged(int index);

    Qt::Orientation _orientation;

    // Non-owning pointers to things that need to be re-translated when the language changes
    QLabel* _languageLabel;
    QLabel* _unitSystemLabel;
    QLabel* _navigationStyleLabel;
    QComboBox* _languageComboBox;
    QComboBox* _unitSystemComboBox;
    QComboBox* _navigationStyleComboBox;
};

}  // namespace StartGui

#endif  // FREECAD_START_GENERALSETTINGSWIDGET_H
