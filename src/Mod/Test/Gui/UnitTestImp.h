#ifndef TESTGUI_UNITTESTIMP_H
#define TESTGUI_UNITTESTIMP_H

#include <QDialog>
#include <memory>

#include <Base/PyObjectBase.h>

class QTreeWidgetItem;

namespace TestGui
{
class Ui_UnitTest;

class UnitTestDialog: public QDialog
{
    Q_OBJECT

public:
    void showErrorDialog(const char* title, const char* message);
    void addUnitTest(const QString& unit);
    void setUnitTest(const QString& unit);
    void clearUnitTests();
    QString getUnitTest() const;
    bool runCurrentTest();
    void setStatusText(const QString& text);
    void setProgressFraction(float fraction, const QString& = QString());
    void clearErrorList();
    void insertError(const QString& failure, const QString& details);
    void setRunCount(int);
    void setFailCount(int);
    void setErrorCount(int);
    void setRemainCount(int);
    void reset();
    void reject() override;

    static UnitTestDialog* instance();
    static void destruct();
    static bool hasInstance();

protected:
    explicit UnitTestDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~UnitTestDialog() override;
    void setProgressColor(const QColor& col);

private:
    void onTreeViewFailureItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onHelpButtonClicked();
    void onAboutButtonClicked();
    void onCopyButtonClicked();
    void onStartButtonClicked();
    void setupConnections();

private:
    std::unique_ptr<Ui_UnitTest> ui;
    static UnitTestDialog* _instance;
};

}  // namespace TestGui


#endif  // TESTGUI_UNITTESTIMP_H
