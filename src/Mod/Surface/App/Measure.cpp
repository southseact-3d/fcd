//! a class for establishing our connection with the unified measurement facility
//! we are treating surfaces like Part objects for now


#include <App/Application.h>
#include <App/MeasureManager.h>
#include "Base/Console.h"
#include "Measure.h"


void Surface::Measure::initialize()
{
    const App::MeasureHandler& handler = App::MeasureManager::getMeasureHandler("Part");

    App::MeasureManager::addMeasureHandler("Surface", handler.typeCb);
}
