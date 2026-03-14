#ifndef PARTDESIGN_Loft_H
#define PARTDESIGN_Loft_H

#include "FeatureSketchBased.h"

namespace PartDesign
{

class PartDesignExport Loft: public ProfileBased
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Loft);

public:
    Loft();

    App::PropertyLinkSubList Sections;
    App::PropertyBool Ruled;
    App::PropertyBool Closed;

    /** @name methods override feature */
    //@{
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderLoft";
    }
    //@}

    static std::vector<Part::TopoShape> getSectionShape(
        const char* name,
        App::DocumentObject* obj,
        const std::vector<std::string>& subname,
        size_t expected_size = 0
    );

protected:
    // handle changed property
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;

private:
    // static const char* TypeEnums[];
    // static const char* SideEnums[];
};

class PartDesignExport AdditiveLoft: public Loft
{

    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::AdditiveLoft);

public:
    AdditiveLoft();
};

class PartDesignExport SubtractiveLoft: public Loft
{

    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::SubtractiveLoft);

public:
    SubtractiveLoft();
};

}  // namespace PartDesign


#endif  // PART_Loft_H
