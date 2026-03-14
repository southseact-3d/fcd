#ifndef PARTDESIGN_DATUMCS_H
#define PARTDESIGN_DATUMCS_H

#include <Mod/Part/App/DatumFeature.h>
#include <Mod/PartDesign/PartDesignGlobal.h>

namespace PartDesign
{

class PartDesignExport CoordinateSystem: public Part::Datum
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::CoordinateSystem);

public:
    CoordinateSystem();
    ~CoordinateSystem() override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderDatumCoordinateSystem";
    }

    Base::Vector3d getXAxis();
    Base::Vector3d getYAxis();
    Base::Vector3d getZAxis();

    App::DocumentObject* getSubObject(
        const char* subname,
        PyObject** pyObj,
        Base::Matrix4D* pmat,
        bool transform,
        int depth
    ) const override;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_DATUMCS_H
