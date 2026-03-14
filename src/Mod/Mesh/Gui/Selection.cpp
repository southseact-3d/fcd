#include "Selection.h"
#include "ui_Selection.h"


using namespace MeshGui;

/* TRANSLATOR MeshGui::Selection */

Selection::Selection(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui_Selection())
{
    ui->setupUi(this);
    setupConnections();
    ui->addSelection->installEventFilter(this);
    ui->clearSelection->installEventFilter(this);

    meshSel.setCheckOnlyVisibleTriangles(ui->visibleTriangles->isChecked());
    meshSel.setCheckOnlyPointToUserTriangles(ui->screenTriangles->isChecked());
    meshSel.setEnabledViewerSelection(false);
}

/*
 *  Destroys the object and frees any allocated resources
 */
Selection::~Selection()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
    meshSel.clearSelection();
    meshSel.setEnabledViewerSelection(true);
}

void Selection::setupConnections()
{
    // clang-format off
    connect(ui->addSelection, &QPushButton::clicked,
            this, &Selection::onAddSelectionClicked);
    connect(ui->clearSelection, &QPushButton::clicked,
            this, &Selection::onClearSelectionClicked);
    connect(ui->visibleTriangles, &QPushButton::clicked,
            this, &Selection::onVisibleTrianglesToggled);
    connect(ui->screenTriangles, &QPushButton::clicked,
            this, &Selection::onScreenTrianglesToggled);
    // clang-format on
}

void Selection::setObjects(const std::vector<Gui::SelectionObject>& o)
{
    meshSel.setObjects(o);
}

std::vector<App::DocumentObject*> Selection::getObjects() const
{
    return meshSel.getObjects();
}

bool Selection::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::HoverEnter) {
        if (o == ui->addSelection) {
            ui->label->setText(tr("Use a brush tool to select the area"));
        }
        else if (o == ui->clearSelection) {
            ui->label->setText(tr("Clears completely the selected area"));
        }
    }
    else if (e->type() == QEvent::HoverLeave) {
        if (o == ui->addSelection) {
            ui->label->clear();
        }
        else if (o == ui->clearSelection) {
            ui->label->clear();
        }
    }

    return false;
}

void Selection::onAddSelectionClicked()
{
    meshSel.startSelection();
}

void Selection::onClearSelectionClicked()
{
    meshSel.clearSelection();
}

void Selection::onVisibleTrianglesToggled(bool on)
{
    meshSel.setCheckOnlyVisibleTriangles(on);
}

void Selection::onScreenTrianglesToggled(bool on)
{
    meshSel.setCheckOnlyPointToUserTriangles(on);
}

#include "moc_Selection.cpp"
