#ifndef PARTDESIGN_DATUMLINE_H
#define PARTDESIGN_DATUMLINE_H

#include <Mod/Part/App/DatumFeature.h>
#include <Mod/PartDesign/PartDesignGlobal.h>
#include <App/PropertyUnits.h>

namespace PartDesign
{

class PartDesignExport Line: public Part::Datum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Line);

public:
    Line();
    ~Line() override;

    App::PropertyEnumeration ResizeMode;
    App::PropertyLength Length;
    void onChanged(const App::Property* prop) override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderDatumLine";
    }

    Base::Vector3d getDirection() const;

private:
    static const char* ResizeModeEnums[];
};

}  // namespace PartDesign


#endif  // PARTDESIGN_DATUMLINE_H
