#ifndef APP_SERVICES_H
#define APP_SERVICES_H

#include "DocumentObject.h"

#include <optional>
#include <Base/Placement.h>

namespace App
{

/**
* This service should provide placement of given sub object (like for example face).
* This feature is not implemented in the core and so it must be provided by module.
*/
class SubObjectPlacementProvider
{
public:
    virtual ~SubObjectPlacementProvider() = default;

    /**
    * Returns placement of sub object relative to the base placement.
    */
    virtual Base::Placement calculate(SubObjectT object, Base::Placement basePlacement) const = 0;
};

/**
* This service should provide center of mass calculation;
*/
class CenterOfMassProvider
{
public:
    virtual ~CenterOfMassProvider() = default;

    virtual bool supports(DocumentObject* object) const = 0;
    virtual std::optional<Base::Vector3d> ofDocumentObject(DocumentObject* object) const = 0;
};

/**
* Default implementation for the center of mass contract
* It always returns empty optional
*/
class NullCenterOfMass final : public CenterOfMassProvider
{
public:
    std::optional<Base::Vector3d> ofDocumentObject(DocumentObject* object) const override;
    bool supports(DocumentObject* object) const override;
};

}


#endif // APP_SERVICES_H
