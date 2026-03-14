#include <QEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QSizePolicy>

#include "Dialogs/DlgCustomizeImp.h"
#include "MainWindow.h"
#include "WidgetFactory.h"


using namespace Gui::Dialog;

QList<QByteArray> DlgCustomizeImp::_pages;

/* TRANSLATOR Gui::Dialog::DlgCustomizeImp */

/**
 *  Constructs a DlgCustomizeImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgCustomizeImp::DlgCustomizeImp(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setModal(false);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    resize(690, 365);

    setWindowTitle(tr("Customize"));
    setSizeGripEnabled(true);

    customLayout = new QGridLayout(this);
    customLayout->setSpacing(6);
    customLayout->setContentsMargins(11, 11, 11, 11);

    layout = new QHBoxLayout;
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    buttonHelp = new QPushButton(this);
    buttonHelp->setText(tr("&Help"));
    layout->addWidget(buttonHelp);

    auto spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(spacer);

    buttonClose = new QPushButton(this);
    buttonClose->setText(tr("&Close"));
    layout->addWidget(buttonClose);

    customLayout->addLayout(layout, 1, 0);

    tabWidget = new QTabWidget(this);
    tabWidget->setObjectName(QStringLiteral("Gui__Dialog__TabWidget"));  // so we can find it in
                                                                         // DlgMacroExecuteImp

    // make sure that pages are ready to create
    GetWidgetFactorySupplier();
    for (const QByteArray& it : _pages) {
        addPage(WidgetFactory().createWidget(it.constData()));
    }

    customLayout->addWidget(tabWidget, 0, 0);


    // tab order
    setTabOrder(tabWidget, buttonClose);
    setTabOrder(buttonClose, buttonHelp);

    // connections
    //
    connect(buttonHelp, &QPushButton::clicked, getMainWindow(), &MainWindow::whatsThis);
    connect(buttonClose, &QPushButton::clicked, this, &QDialog::close);
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgCustomizeImp::~DlgCustomizeImp() = default;

/**
 * Adds a customize page with its class name \a className.
 * To create this page it must be registered in the WidgetFactory.
 * @see WidgetFactory
 * @see CustomPageProducer
 */
void DlgCustomizeImp::addPage(const char* className)
{
    _pages.push_back(className);
}

/** Inserts a new tab page with its caption */
void DlgCustomizeImp::addPage(QWidget* w)
{
    tabWidget->addTab(w, w->windowTitle());
}

void DlgCustomizeImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        setWindowTitle(tr("Customize"));
        buttonHelp->setText(tr("&Help"));
        buttonClose->setText(tr("&Close"));
        int count = tabWidget->count();
        for (int i = 0; i < count; i++) {
            QWidget* w = tabWidget->widget(i);
            tabWidget->setTabText(i, w->windowTitle());
        }
    }
    QDialog::changeEvent(e);
}

#include "moc_DlgCustomizeImp.cpp"
