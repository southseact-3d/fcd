#ifndef POINTS_TOOLS_H
#define POINTS_TOOLS_H

#include <App/DocumentObject.h>
#include <algorithm>

namespace Points
{

template<typename PropertyT>
bool copyProperty(
    App::DocumentObject* target,
    std::vector<App::DocumentObject*> source,
    const char* propertyName
)
{
    // check for properties
    if (std::all_of(std::begin(source), std::end(source), [=](auto obj) {
            return freecad_cast<PropertyT*>(obj->getPropertyByName(propertyName)) != nullptr;
        })) {

        auto target_prop = freecad_cast<PropertyT*>(
            target->addDynamicProperty(PropertyT::getClassTypeId().getName(), propertyName)
        );
        if (target_prop) {
            auto values = target_prop->getValues();
            for (auto it : source) {
                auto source_prop = freecad_cast<PropertyT*>(it->getPropertyByName(propertyName));
                if (source_prop) {
                    auto source_values = source_prop->getValues();
                    values.insert(values.end(), source_values.begin(), source_values.end());
                }
            }

            target_prop->setValues(values);
            return true;
        }
    }

    return false;
}

}  // namespace Points

#endif  // POINTS_TOOLS_H
