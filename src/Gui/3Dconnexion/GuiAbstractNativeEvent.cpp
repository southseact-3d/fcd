#include "GuiAbstractNativeEvent.h"

#include "GuiApplicationNativeEventAware.h"

std::vector<int> Gui::GuiAbstractNativeEvent::motionDataArray(6, 0);
Gui::GUIApplicationNativeEventAware* Gui::GuiAbstractNativeEvent::mainApp;

Gui::GuiAbstractNativeEvent::GuiAbstractNativeEvent(GUIApplicationNativeEventAware* app)
    : QObject(app)
{
    mainApp = app;
}

Gui::GuiAbstractNativeEvent::~GuiAbstractNativeEvent()
{}

#include "3Dconnexion/moc_GuiAbstractNativeEvent.cpp"
