#include <QMessageBox>


#include <App/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/WaitCursor.h>

#include "Poisson.h"
#include "ui_Poisson.h"


using namespace ReenGui;

class PoissonWidget::Private
{
public:
    Ui_PoissonWidget ui;
    App::DocumentObjectT obj;
    Private() = default;
    ~Private() = default;
};

/* TRANSLATOR ReenGui::PoissonWidget */

PoissonWidget::PoissonWidget(const App::DocumentObjectT& obj, QWidget* parent)
    : d(new Private())
{
    Q_UNUSED(parent);
    d->ui.setupUi(this);
    d->obj = obj;
}

PoissonWidget::~PoissonWidget()
{
    delete d;
}

bool PoissonWidget::accept()
{
    try {
        QString document = QString::fromStdString(d->obj.getDocumentPython());
        QString object = QString::fromStdString(d->obj.getObjectPython());

        QString argument = QStringLiteral(
                               "Points=%1.Points, "
                               "OctreeDepth=%2, "
                               "SolverDivide=%3, "
                               "SamplesPerNode=%4"
        )
                               .arg(object)
                               .arg(d->ui.octreeDepth->value())
                               .arg(d->ui.solverDivide->value())
                               .arg(d->ui.samplesPerNode->value());
        QString command = QStringLiteral(
                              "%1.addObject(\"Mesh::Feature\", \"Poisson\").Mesh = "
                              "ReverseEngineering.poissonReconstruction(%2)"
        )
                              .arg(document, argument);

        Gui::WaitCursor wc;
        Gui::Command::addModule(Gui::Command::App, "ReverseEngineering");
        Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Poisson reconstruction"));
        Gui::Command::runCommand(Gui::Command::Doc, command.toLatin1());
        Gui::Command::commitCommand();
        Gui::Command::updateActive();
    }
    catch (const Base::Exception& e) {
        Gui::Command::abortCommand();
        QMessageBox::warning(this, tr("Input error"), QString::fromLatin1(e.what()));
        return false;
    }

    return true;
}

void PoissonWidget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        d->ui.retranslateUi(this);
    }
}


/* TRANSLATOR ReenGui::TaskPoisson */

TaskPoisson::TaskPoisson(const App::DocumentObjectT& obj)
{
    widget = new PoissonWidget(obj);
    addTaskBox(Gui::BitmapFactory().pixmap("actions/FitSurface"), widget);
}

TaskPoisson::~TaskPoisson() = default;

void TaskPoisson::open()
{}

bool TaskPoisson::accept()
{
    return widget->accept();
}

#include "moc_Poisson.cpp"
