#ifndef PARTDESIGN_FeatureBase_H
#define PARTDESIGN_FeatureBase_H

#include "Feature.h"

/// Base class of all additive features in PartDesign
namespace PartDesign
{

class PartDesignExport FeatureBase: public PartDesign::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FeatureBase);

public:
    FeatureBase();

    short int mustExecute() const override;

    Part::Feature* getBaseObject(bool silent = false) const override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderBase";
    }

    void onChanged(const App::Property* prop) override;
    App::DocumentObjectExecReturn* execute() override;
    void onDocumentRestored() override;

private:
    void trySetBaseFeatureOfBody();
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureBase_H
