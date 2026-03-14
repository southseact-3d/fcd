#ifndef PART_FEATUREPARTBOX_H
#define PART_FEATUREPARTBOX_H

#include <App/PropertyStandard.h>

#include <Mod/Part/PartGlobal.h>

#include "PrimitiveFeature.h"


namespace Part
{

class PartExport Box: public Part::Primitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Box);

public:
    Box();

    App::PropertyLength Length, Height, Width;


    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderBox";
    }

protected:
    void Restore(Base::XMLReader& reader) override;
    /// get called by the container when a property has changed
    void onChanged(const App::Property* prop) override;
    //@}
};

}  // namespace Part


#endif  // PART_FEATUREPARTBOX_H
