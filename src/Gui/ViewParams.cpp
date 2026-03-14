#include <App/Application.h>
#include "ViewParams.h"

using namespace Gui;

ViewParams::ViewParams()
{
    handle = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View");
    handle->Attach(this);
#undef FC_VIEW_PARAM
#define FC_VIEW_PARAM(_name, _ctype, _type, _def) _name = handle->Get##_type(#_name, _def);

    FC_VIEW_PARAMS
}

ViewParams::~ViewParams() = default;

void ViewParams::OnChange(Base::Subject<const char*>&, const char* sReason)
{
    if (!sReason) {
        return;
    }
#undef FC_VIEW_PARAM
#define FC_VIEW_PARAM(_name, _ctype, _type, _def) \
    if (strcmp(sReason, #_name) == 0) { \
        _name = handle->Get##_type(#_name, _def); \
        return; \
    }
    FC_VIEW_PARAMS
}

ViewParams* ViewParams::instance()
{
    static ViewParams* inst;
    if (!inst) {
        inst = new ViewParams;
    }
    return inst;
}
