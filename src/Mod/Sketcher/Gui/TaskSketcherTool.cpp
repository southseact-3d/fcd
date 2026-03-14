#include "TaskSketcherTool.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Tools.h>
#include <Base/UnitsApi.h>

#include <QEvent>

#include "ViewProviderSketch.h"

#include "SketcherToolDefaultWidget.h"

using namespace SketcherGui;
using namespace Gui::TaskView;

TaskSketcherTool::TaskSketcherTool(ViewProviderSketch* sketchView)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"), tr("Tool Parameters"), true, nullptr)
    , sketchView(sketchView)
{}

TaskSketcherTool::~TaskSketcherTool()
{}

void TaskSketcherTool::toolChanged(const std::string& toolname)
{
    Q_UNUSED(toolname)

    widget = sketchView->toolManager.createToolWidget();

    if (widget) {
        this->groupLayout()->addWidget(widget.get());

        setHeaderText(sketchView->toolManager.getToolWidgetText());
        setHeaderIcon(sketchView->toolManager.getToolIcon());

        signalToolWidgetChanged(this->widget.get());
    }
    else {
        signalToolWidgetChanged(nullptr);
    }
}

#include "moc_TaskSketcherTool.cpp"
