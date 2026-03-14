#include <QEvent>
#include <QGridLayout>
#include <QSplitter>

#include "ComboView.h"
#include "PropertyView.h"
#include "Tree.h"


using namespace Gui;
using namespace Gui::DockWnd;


/* TRANSLATOR Gui::DockWnd::ComboView */

ComboView::ComboView(Gui::Document* pcDocument, QWidget* parent)
    : DockWindow(pcDocument, parent)
{
    auto pLayout = new QGridLayout(this);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);

    // tabs to switch between Tree/Properties and TaskPanel
    auto splitter = new QSplitter();
    pLayout->addWidget(splitter, 0, 0);

    // splitter between tree and property view
    splitter->setOrientation(Qt::Vertical);

    tree = new TreePanel("ComboView", this);
    splitter->addWidget(tree);

    // property view
    prop = new PropertyView(this);
    splitter->addWidget(prop);
}

ComboView::~ComboView() = default;

#include "moc_ComboView.cpp"
