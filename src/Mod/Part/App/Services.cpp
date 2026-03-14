#include <Base/Vector3D.h>

#include "Services.h"

AttacherSubObjectPlacement::AttacherSubObjectPlacement()
    : attacher(std::make_unique<Attacher::AttachEngine3D>())
{
    attacher->setUp({}, Attacher::mmMidpoint);
}

Base::Placement AttacherSubObjectPlacement::calculate(
    App::SubObjectT object,
    Base::Placement basePlacement
) const
{
    attacher->setReferences({object});

    auto calculatedAttachment = attacher->calculateAttachedPlacement(basePlacement);

    return basePlacement.inverse() * calculatedAttachment;
}

std::optional<Base::Vector3d> PartCenterOfMass::ofDocumentObject(App::DocumentObject* object) const
{
    if (const auto* feature = freecad_cast<Part::Feature*>(object)) {
        const auto shape = feature->Shape.getShape();

        if (const auto cog = shape.centerOfGravity()) {
            const Base::Placement comPlacement {*cog, Base::Rotation {}};

            return (feature->Placement.getValue().inverse() * comPlacement).getPosition();
        }
    }

    return {};
}

bool PartCenterOfMass::supports(App::DocumentObject* object) const
{
    return object->isDerivedFrom<Part::Feature>();
}
