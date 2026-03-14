#include "ParamHandler.h"

using namespace Gui;

ParamHandlers::ParamHandlers()
{}

ParamHandlers::~ParamHandlers()
{}

void ParamHandlers::addHandler(const ParamKey& key, const std::shared_ptr<ParamHandler>& handler)
{
    if (handlers.empty()) {
        conn = App::GetApplication().GetUserParameter().signalParamChanged.connect(
            [this](ParameterGrp* Param, ParameterGrp::ParamType, const char* Name, const char*) {
                if (!Param || !Name) {
                    return;
                }
                auto it = handlers.find(ParamKey(Param, Name));
                if (it != handlers.end() && it->second->onChange(&it->first)) {
                    pendings.insert(it->second);
                    timer.start(100);
                }
            }
        );

        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, [this]() {
            for (const auto& v : pendings) {
                v->onTimer();
            }
            pendings.clear();
        });
    }
    handlers[key] = handler;
}
