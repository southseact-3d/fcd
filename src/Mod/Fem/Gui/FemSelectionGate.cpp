#include <Base/Tools.h>

#include "FemSelectionGate.h"


using namespace FemGui;
using namespace Gui;

bool FemSelectionGate::allow(App::Document* /*pDoc*/, App::DocumentObject* /*pObj*/, const char* sSubName)
{
    if (Base::Tools::isNullOrEmpty(sSubName)) {
        return false;
    }

    if (sSubName[0] == 'E' && sSubName[1] == 'l' && sSubName[2] == 'e' && sSubName[3] == 'm'
        && (Type == Element || Type == NodeElement)) {
        return true;
    }
    if (sSubName[0] == 'N' && sSubName[1] == 'o' && sSubName[2] == 'd' && sSubName[3] == 'e'
        && (Type == Node || Type == NodeElement)) {
        return true;
    }

    return false;
}
