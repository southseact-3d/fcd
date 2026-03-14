#include "SoFCColorBarNotifier.h"
#include "SoFCColorBar.h"
#include "Window.h"

using namespace Gui;

SoFCColorBarNotifier& SoFCColorBarNotifier::instance()
{
    static SoFCColorBarNotifier instance;
    return instance;
}

SoFCColorBarNotifier::SoFCColorBarNotifier()
{
    group = Gui::WindowParameter::getDefaultParameter()->GetGroup("View");
    group->Attach(this);
}

void SoFCColorBarNotifier::attach(SoFCColorBarBase* bar)
{
    if (bars.insert(bar).second) {
        bar->ref();
        group->Notify("CbLabelTextSize");
    }
}

void SoFCColorBarNotifier::detach(SoFCColorBarBase* bar)
{
    auto pos = bars.find(bar);
    if (pos != bars.end()) {
        bars.erase(pos);
        bar->unref();
    }
}

void SoFCColorBarNotifier::OnChange(ParameterGrp::SubjectType& caller, ParameterGrp::MessageType reason)
{
    const ParameterGrp& grp = dynamic_cast<ParameterGrp&>(caller);
    if (strcmp(reason, "CbLabelTextSize") == 0 || strcmp(reason, "CbLabelColor") == 0) {
        SoLabelTextFormat format;
        format.textSize = static_cast<int>(grp.GetInt("CbLabelTextSize", format.textSize));
        format.textColor = static_cast<uint32_t>(grp.GetUnsigned("CbLabelColor", format.textColor));

        for (auto bar : bars) {
            bar->setFormat(format);
        }
    }
}
