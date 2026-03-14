#ifndef PARTDESIGN_DATUMPLANE_H
#define PARTDESIGN_DATUMPLANE_H

#include <Mod/Part/App/DatumFeature.h>
#include <Mod/PartDesign/PartDesignGlobal.h>
#include <App/PropertyUnits.h>

namespace PartDesign
{

class PartDesignExport Plane: public Part::Datum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Plane);

public:
    Plane();
    ~Plane() override;

    App::PropertyEnumeration ResizeMode;
    App::PropertyLength Length;
    App::PropertyLength Width;

    void onChanged(const App::Property* prop) override;
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderDatumPlane";
    }

    Base::Vector3d getNormal();

protected:
    void Restore(Base::XMLReader& reader) override;

private:
    static const char* ResizeModeEnums[];
};

}  // namespace PartDesign


#endif  // PARTDESIGN_DATUMPLANE_H
