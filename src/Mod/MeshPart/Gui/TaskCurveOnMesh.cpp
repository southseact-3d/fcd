#include <Gui/View3DInventor.h>

#include "CurveOnMesh.h"
#include "TaskCurveOnMesh.h"
#include "ui_TaskCurveOnMesh.h"


using namespace MeshPartGui;

CurveOnMeshWidget::CurveOnMeshWidget(Gui::View3DInventor* view, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui_TaskCurveOnMesh())
    , myCurveHandler(new CurveOnMeshHandler(this))
    , myView(view)
{
    ui->setupUi(this);
    connect(ui->startButton, &QPushButton::clicked, this, &CurveOnMeshWidget::onStartButtonClicked);
    this->setup();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CurveOnMeshWidget::~CurveOnMeshWidget()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
}

void CurveOnMeshWidget::setup()
{
    ui->meshTolerance->setValue(0.2);
    ui->continuity->addItem(QStringLiteral("C0"), static_cast<int>(GeomAbs_C0));
    ui->continuity->addItem(QStringLiteral("C1"), static_cast<int>(GeomAbs_C1));
    ui->continuity->addItem(QStringLiteral("C2"), static_cast<int>(GeomAbs_C2));
    ui->continuity->addItem(QStringLiteral("C3"), static_cast<int>(GeomAbs_C3));
    ui->continuity->setCurrentIndex(2);

    for (int i = 0; i < 8; i++) {
        ui->maxDegree->addItem(QString::number(i + 1));
    }
    ui->maxDegree->setCurrentIndex(4);
}

void CurveOnMeshWidget::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void CurveOnMeshWidget::onStartButtonClicked()
{
    int cont = ui->continuity->itemData(ui->continuity->currentIndex()).toInt();
    myCurveHandler->enableApproximation(ui->groupBox_2->isChecked());
    myCurveHandler->setParameters(
        ui->maxDegree->currentIndex() + 1,
        static_cast<GeomAbs_Shape>(cont),
        ui->meshTolerance->value(),
        ui->splitAngle->value().getValue()
    );
    myCurveHandler->enableCallback(myView);
}

void CurveOnMeshWidget::reject()
{
    myCurveHandler->recomputeDocument();
}

// ----------------------------------------------------------------------------

TaskCurveOnMesh::TaskCurveOnMesh(Gui::View3DInventor* view)
{
    widget = new CurveOnMeshWidget(view);
    addTaskBox(widget);
}

bool TaskCurveOnMesh::reject()
{
    widget->reject();
    return true;
}

#include "moc_TaskCurveOnMesh.cpp"
