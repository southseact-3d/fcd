#ifndef GUI_ASSISTANT_H
#define GUI_ASSISTANT_H

#include <QObject>

class QProcess;

namespace Gui
{

class Assistant: public QObject
{
    Q_OBJECT

public:
    Assistant();
    ~Assistant() override;
    void showDocumentation(const QString& file);

private Q_SLOTS:
    void readyReadStandardOutput();
    void readyReadStandardError();

private:
    bool startAssistant();
    QProcess* proc;
};

}  // namespace Gui

#endif  // GUI_ASSISTANT_H
