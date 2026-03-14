#ifndef PATH_DlgProcessorChooser_H
#define PATH_DlgProcessorChooser_H

#include <QDialog>


class Ui_DlgProcessorChooser;

namespace PathGui
{

class DlgProcessorChooser: public QDialog
{
    Q_OBJECT

public:
    explicit DlgProcessorChooser(std::vector<std::string>& scriptnames, bool withArguments = false);
    ~DlgProcessorChooser() override;

    std::string getProcessor();
    std::string getArguments();

    void accept() override;

protected Q_SLOTS:

private:
    Ui_DlgProcessorChooser* ui;
    std::string processor, arguments;
};

}  // namespace PathGui

#endif  // PATH_DlgProcessorChooser_H
