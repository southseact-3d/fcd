#ifndef APP_MEASUREPOSITION_H
#define APP_MEASUREPOSITION_H

#include <Mod/Measure/MeasureGlobal.h>

#include <QTextStream>

#include <App/DocumentObject.h>
#include <App/PropertyGeo.h>
#include <App/PropertyUnits.h>

#include <Mod/Part/App/MeasureInfo.h>

#include "MeasureBase.h"


namespace Measure
{


class MeasureExport MeasurePosition: public Measure::MeasureBaseExtendable<Part::MeasurePositionInfo>
{
    PROPERTY_HEADER_WITH_OVERRIDE(Measure::MeasurePosition);

public:
    /// Constructor
    MeasurePosition();
    ~MeasurePosition() override;

    App::PropertyLinkSub Element;
    App::PropertyPosition Position;

    App::DocumentObjectExecReturn* execute() override;

    const char* getViewProviderName() const override
    {
        return "MeasureGui::ViewProviderMeasurePosition";
    }

    static bool isValidSelection(const App::MeasureSelection& selection);
    void parseSelection(const App::MeasureSelection& selection) override;

    std::vector<std::string> getInputProps() override
    {
        return {"Element"};
    }
    App::Property* getResultProp() override
    {
        return &this->Position;
    }
    QString getResultString() override;

    Base::Placement getPlacement() const override;

    // Return the object we are measuring
    std::vector<App::DocumentObject*> getSubject() const override;

private:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Measure


#endif  // APP_MEASUREPOSITION_H
