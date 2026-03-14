#include <QEvent>


#include <Gui/BitmapFactory.h>

#include "TaskSketcherCreateCommands.h"


using namespace Gui::TaskView;

TaskSketcherCreateCommands::TaskSketcherCreateCommands(QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"), tr("Appearance"), true, parent)
{
    // we need a separate container widget to add all controls to
    // proxy = new QWidget(this);
    // ui = new Ui_TaskAppearance();
    // ui->setupUi(proxy);
    // ui->textLabel1_3->hide();
    // ui->changePlot->hide();
    // QMetaObject::connectSlotsByName(this);

    // this->groupLayout()->addWidget(proxy);

    // std::vector<Gui::ViewProvider*> views;
    // setDisplayModes(views);
    // setPointSize(views);
    // setLineWidth(views);
    // setTransparency(views);
    Gui::Selection().Attach(this);
}

TaskSketcherCreateCommands::~TaskSketcherCreateCommands()
{
    // delete ui;
    Gui::Selection().Detach(this);
}

void TaskSketcherCreateCommands::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    // if (e->type() == QEvent::LanguageChange) {
    //     ui->retranslateUi(proxy);
    // }
}

/// @cond DOXERR
void TaskSketcherCreateCommands::OnChange(
    Gui::SelectionSingleton::SubjectType& rCaller,
    Gui::SelectionSingleton::MessageType Reason
)
{
    Q_UNUSED(rCaller);
    if (Reason.Type == SelectionChanges::AddSelection || Reason.Type == SelectionChanges::RmvSelection
        || Reason.Type == SelectionChanges::SetSelection
        || Reason.Type == SelectionChanges::ClrSelection) {
    }
}
/// @endcond DOXERR


#include "moc_TaskSketcherCreateCommands.cpp"
