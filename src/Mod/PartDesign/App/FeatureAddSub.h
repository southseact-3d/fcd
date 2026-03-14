#ifndef PARTDESIGN_FeatureAdditive_H
#define PARTDESIGN_FeatureAdditive_H

#include "FeatureRefine.h"

#include <QtCore>

/// Base class of all additive features in PartDesign
namespace PartDesign
{

class PartDesignExport FeatureAddSub: public PartDesign::FeatureRefine
{
    Q_DECLARE_TR_FUNCTIONS(PartDesign::FeatureAddSub)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FeatureAddSub);

public:
    enum Type
    {
        Additive = 0,
        Subtractive
    };

    FeatureAddSub();

    void onChanged(const App::Property*) override;
    Type getAddSubType();

    short mustExecute() const override;

    virtual void getAddSubShape(Part::TopoShape& addShape, Part::TopoShape& subShape);

    void updatePreviewShape() override;

    Part::PropertyPartShape AddSubShape;


protected:
    Type addSubType {Additive};
};

using FeatureAddSubPython = App::FeaturePythonT<FeatureAddSub>;

class FeatureAdditivePython: public FeatureAddSubPython
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FeatureAdditivePython);

public:
    FeatureAdditivePython();
    ~FeatureAdditivePython() override;
};

class FeatureSubtractivePython: public FeatureAddSubPython
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FeatureSubtractivePython);

public:
    FeatureSubtractivePython();
    ~FeatureSubtractivePython() override;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureAdditive_H
