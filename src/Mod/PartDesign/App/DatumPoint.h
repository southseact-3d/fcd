#ifndef PARTDESIGN_DATUMPOINT_H
#define PARTDESIGN_DATUMPOINT_H

#include <Mod/Part/App/DatumFeature.h>
#include <Mod/PartDesign/PartDesignGlobal.h>

namespace PartDesign
{

class PartDesignExport Point: public Part::Datum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Point);

public:
    Point();
    ~Point() override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderDatumPoint";
    }

    Base::Vector3d getPoint();

    using Superclass = Part::Datum;

protected:
    void onChanged(const App::Property* prop) override;
    void onDocumentRestored() override;

private:
    void makeShape();
};

}  // namespace PartDesign


#endif  // PARTDESIGN_DATUMPOINT_H
