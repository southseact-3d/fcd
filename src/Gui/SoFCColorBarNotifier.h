#ifndef GUI_SOFCCOLORBARNOTIFIER_H
#define GUI_SOFCCOLORBARNOTIFIER_H

#include <Base/Parameter.h>
#include <set>

namespace Gui
{

class SoFCColorBarBase;

class GuiExport SoFCColorBarNotifier: public ParameterGrp::ObserverType  // NOLINT
{
public:
    static SoFCColorBarNotifier& instance();
    void attach(SoFCColorBarBase* bar);
    void detach(SoFCColorBarBase* bar);
    void OnChange(ParameterGrp::SubjectType& caller, ParameterGrp::MessageType reason) override;

private:
    SoFCColorBarNotifier();
    ~SoFCColorBarNotifier() override = default;

    FC_DISABLE_COPY_MOVE(SoFCColorBarNotifier)

private:
    std::set<SoFCColorBarBase*> bars;
    ParameterGrp::handle group;
};

}  // namespace Gui


#endif  // GUI_SOFCCOLORBARNOTIFIER_H
