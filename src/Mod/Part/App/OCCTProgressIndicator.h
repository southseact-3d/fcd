#ifndef PART_OCCTPROGRESSINDICATOR_H
#define PART_OCCTPROGRESSINDICATOR_H

#include <App/Application.h>
#include <Base/ProgressIndicator.h>
#include <Mod/Part/PartGlobal.h>

#include <Message_ProgressIndicator.hxx>

namespace Part
{

class PartExport OCCTProgressIndicator: public Message_ProgressIndicator
{
    Base::ProgressIndicator& baseIndicator;

public:
    OCCTProgressIndicator(Base::ProgressIndicator& indicator)
        : baseIndicator(indicator)
    {}

    Standard_Boolean UserBreak() override
    {
        return baseIndicator.userBreak();
    }

    void Show(const Message_ProgressScope& scope, const Standard_Boolean isForce) override
    {
        float pos = -1;  // negative means indeterminate
        if (!scope.IsInfinite()) {
            pos = static_cast<float>(GetPosition());
        }
        using ShowFlags = Base::ProgressIndicator::ShowFlags;
        baseIndicator.show(pos, isForce ? ShowFlags::Force : ShowFlags::None);
    }

    static OCCTProgressIndicator getAppIndicator()
    {
        return {App::GetApplication().getProgressIndicator()};
    }
};


#if OCC_VERSION_HEX < 0x070600
// Stubs out OCCT Message_ProgressRange for OCCT versions below 7.5
class Message_ProgressRange
{
public:
    bool UserBreak()
    {
        return false;
    }
    void Show([[maybe_unused]] float position, [[maybe_unused]] bool isForce)
    {}
};
#endif

}  // namespace Part

#endif  // PART_OCCTPROGRESSINDICATOR_H
