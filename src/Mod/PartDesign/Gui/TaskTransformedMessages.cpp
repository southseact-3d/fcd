#include <Gui/BitmapFactory.h>

#include "ui_TaskTransformedMessages.h"
#include "TaskTransformedMessages.h"
#include "ViewProviderTransformed.h"

using namespace PartDesignGui;
using namespace Gui::TaskView;
namespace sp = std::placeholders;

TaskTransformedMessages::TaskTransformedMessages(ViewProviderTransformed* transformedView_)
    : TaskBox(
          Gui::BitmapFactory().pixmap("Part_Transformed_Copy"),
          tr("Transformed Feature Messages"),
          true,
          nullptr
      )
    , transformedView(transformedView_)
    , ui(new Ui_TaskTransformedMessages)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    // set a minimum height to avoid a sudden resize and to
    // lose focus of the currently used spin boxes
    ui->labelTransformationStatus->setMinimumHeight(50);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
    ui->labelTransformationStatus->setText(transformedView->getMessage());

    // NOLINTBEGIN
    connectionDiagnosis = transformedView->signalDiagnosis.connect(
        std::bind(&PartDesignGui::TaskTransformedMessages::slotDiagnosis, this, sp::_1)
    );
    // NOLINTEND
}

TaskTransformedMessages::~TaskTransformedMessages()
{
    connectionDiagnosis.disconnect();
}

void TaskTransformedMessages::slotDiagnosis(QString msg)
{
    ui->labelTransformationStatus->setText(msg);
}

#include "moc_TaskTransformedMessages.cpp"
