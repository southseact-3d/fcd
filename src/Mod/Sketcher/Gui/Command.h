#ifndef SKETCHERGUI_CommandConstraints_H
#define SKETCHERGUI_CommandConstraints_H

#include <QWidgetAction>
#include <QCoreApplication>

#include <Base/Parameter.h>

class QCheckBox;
class QLabel;
class QWidget;
class QObject;
class QListWidget;

namespace Gui
{
class Command;
class QuantitySpinBox;
}  // namespace Gui

namespace SketcherGui
{

class ViewProviderSketch;

class GridSpaceAction: public QWidgetAction
{
    Q_DECLARE_TR_FUNCTIONS(SketcherGui::GridSpaceAction)
public:
    GridSpaceAction(QObject* parent);

    void updateWidget();

    void languageChange();

protected:
    QWidget* createWidget(QWidget* parent) override;

private:
    ViewProviderSketch* getView();

    ParameterGrp::handle getParameterPath();

private:
    QCheckBox* gridShow;
    QCheckBox* gridAutoSpacing;
    QCheckBox* snapToGrid;
    QLabel* sizeLabel;
    Gui::QuantitySpinBox* gridSizeBox;
};


class SnapSpaceAction: public QWidgetAction
{
    Q_DECLARE_TR_FUNCTIONS(SketcherGui::SnapSpaceAction)
public:
    SnapSpaceAction(QObject* parent);

    void updateWidget(bool snapenabled);

    void languageChange();

protected:
    QWidget* createWidget(QWidget* parent) override;

private:
    ParameterGrp::handle getParameterPath();

private:
    QCheckBox* snapToObjects;
    QLabel* angleLabel;
    Gui::QuantitySpinBox* snapAngle;
};


class RenderingOrderAction: public QWidgetAction
{
    Q_DECLARE_TR_FUNCTIONS(SketcherGui::RenderingOrderAction)
public:
    RenderingOrderAction(QObject* parent);

    void updateWidget();

    void languageChange();

protected:
    QWidget* createWidget(QWidget* parent) override;

private:
    ParameterGrp::handle getParameterPath();

private:
    QListWidget* list;
};

}  // namespace SketcherGui
#endif  // SKETCHERGUI_Command_H
