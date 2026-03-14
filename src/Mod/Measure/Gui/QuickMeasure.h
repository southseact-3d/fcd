#ifndef MEASUREGUI_QUICKMEASURE_H
#define MEASUREGUI_QUICKMEASURE_H

#include <QObject>

#include <Mod/Measure/MeasureGlobal.h>

#include <Gui/Selection/Selection.h>

class QTimer;

namespace Measure
{
class Measurement;
}

namespace MeasureGui
{

class QuickMeasure: public QObject, Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit QuickMeasure(QObject* parent = nullptr);
    ~QuickMeasure() override;
    void print(const QString& message);

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void tryMeasureSelection();

    bool shouldMeasure(const Gui::SelectionChanges& msg) const;
    void addSelectionToMeasurement();
    bool isObjAcceptable(App::DocumentObject* obj);
    void printResult();

    void processSelection();

    Measure::Measurement* measurement;

    QTimer* selectionTimer;
    bool pendingProcessing;
};

}  // namespace MeasureGui

#endif  // MEASUREGUI_QUICKMEASURE_H
