#include <App/Application.h>
#include <App/MeasureManager.h>
#include "Base/Console.h"
#include "Measure.h"


void PartDesign::Measure::initialize()
{
    const App::MeasureHandler& handler = App::MeasureManager::getMeasureHandler("Part");

    App::MeasureManager::addMeasureHandler("PartDesign", handler.typeCb);
}
