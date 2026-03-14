#ifndef WIZARD_H
#define WIZARD_H


#include <QDialog>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionFactory>

class QLabel;
class QFrame;
class QStackedWidget;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QExtensionManager;

class Wizard: public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_PROPERTY(QString pageTitle READ pageTitle WRITE setPageTitle STORED false)

public:
    Wizard(QWidget* parent = 0);

    QSize sizeHint() const;

    void addPage(QWidget* page);
    void removePage(int index);
    int count() const;
    int currentIndex() const;
    void insertPage(int index, QWidget* page);
    void setCurrentIndex(int index);
    QWidget* widget(int index);

    QPushButton* backButton() const;
    QPushButton* nextButton() const;

    QString pageTitle() const;
    void setPageTitle(QString const& newTitle);

public Q_SLOTS:
    void backButtonClicked();
    void nextButtonClicked();

Q_SIGNALS:
    void currentIndexChanged(int index);
    void pageTitleChanged(const QString& title);

private:
    QLabel* textLabel;
    QFrame* topLine;
    QFrame* bottomLine;
    QStackedWidget* stackWidget;
    QPushButton* _cancelButton;
    QPushButton* _backButton;
    QPushButton* _nextButton;
    QPushButton* _finishButton;
    QHBoxLayout* buttonLayout;
    QVBoxLayout* mainLayout;
};

class WizardExtension: public QObject, public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)

public:
    WizardExtension(Wizard* widget, QObject* parent);

    int count() const;
    QWidget* widget(int index) const;
    int currentIndex() const;
    void setCurrentIndex(int index);
    void addWidget(QWidget* widget);
    void insertWidget(int index, QWidget* widget);
    void remove(int index);

private:
    Wizard* myWidget;
};

class QExtensionManager;

class WizardExtensionFactory: public QExtensionFactory
{
    Q_OBJECT

public:
    WizardExtensionFactory(QExtensionManager* parent = 0);

protected:
    QObject* createExtension(QObject* object, const QString& iid, QObject* parent) const;
};

#endif
