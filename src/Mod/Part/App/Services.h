#ifndef PART_SERVICES_H
#define PART_SERVICES_H

#include <Attacher.h>
#include <App/Services.h>

class AttacherSubObjectPlacement final: public App::SubObjectPlacementProvider
{
public:
    AttacherSubObjectPlacement();

    Base::Placement calculate(App::SubObjectT object, Base::Placement basePlacement) const override;

private:
    std::unique_ptr<Attacher::AttachEngine3D> attacher;
};

class PartCenterOfMass final: public App::CenterOfMassProvider
{
public:
    std::optional<Base::Vector3d> ofDocumentObject(App::DocumentObject* object) const override;
    bool supports(App::DocumentObject* object) const override;
};

#endif  // PART_SERVICES_H
