//! a class for establishing our connection with the unified measurement facility
//! we are treating sketches like Part objects for now


#include <App/Application.h>
#include <App/MeasureManager.h>
#include "Base/Console.h"
#include "Measure.h"


void Sketcher::Measure::initialize()
{
    const App::MeasureHandler& handler = App::MeasureManager::getMeasureHandler("Part");

    App::MeasureManager::addMeasureHandler("Sketcher", handler.typeCb);
}
