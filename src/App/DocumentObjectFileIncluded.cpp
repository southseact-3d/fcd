#include "DocumentObjectFileIncluded.h"

using namespace App;

PROPERTY_SOURCE(App::DocumentObjectFileIncluded, App::DocumentObject)


DocumentObjectFileIncluded::DocumentObjectFileIncluded()
{
    ADD_PROPERTY_TYPE(File,
                      (nullptr),
                      "",
                      (App::PropertyType)(Prop_None),
                      "File to include into Project File");
}

DocumentObjectFileIncluded::~DocumentObjectFileIncluded() = default;
