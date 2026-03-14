#include "Services.h"

std::optional<Base::Vector3d>
App::NullCenterOfMass::ofDocumentObject([[maybe_unused]] DocumentObject* object) const
{
    return std::nullopt;
}

bool App::NullCenterOfMass::supports(DocumentObject* /*object*/) const
{
    return false;
}