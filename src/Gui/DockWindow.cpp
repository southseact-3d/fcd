#include "DockWindow.h"


using namespace Gui;

DockWindow::DockWindow(Gui::Document* pcDocument, QWidget* parent)
    : QWidget(parent)
    , BaseView(pcDocument)
{}

DockWindow::~DockWindow() = default;

#include "moc_DockWindow.cpp"
