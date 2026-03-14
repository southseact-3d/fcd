#include <App/Document.h>
#include <Base/Placement.h>

#include "Origin.h"

using namespace App;

PROPERTY_SOURCE(App::Origin, App::LocalCoordinateSystem)

Origin::Origin()
{
    // App::Origin is a LCS for which placement is fixed to identity.
    Placement.setStatus(Property::Hidden, true);
}

Origin::~Origin() = default;
