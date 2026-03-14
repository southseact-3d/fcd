#include "WorkbenchFactory.h"
#include "Workbench.h"

using namespace Gui;

Gui::WorkbenchFactoryInst* Gui::WorkbenchFactoryInst::_pcSingleton = nullptr;

WorkbenchFactoryInst& WorkbenchFactoryInst::instance()
{
    if (!_pcSingleton) {
        _pcSingleton = new WorkbenchFactoryInst;
    }
    return *_pcSingleton;
}

void WorkbenchFactoryInst::destruct()
{
    delete _pcSingleton;
    _pcSingleton = nullptr;
}

Workbench* WorkbenchFactoryInst::createWorkbench(const char* sName) const
{
    auto obj = (Workbench*)Produce(sName);
    auto wb = freecad_cast<Workbench*>(obj);
    if (!wb) {
        delete obj;  // delete the unknown object as no workbench object
        return nullptr;
    }

    wb->setName(sName);
    return wb;
}

std::list<std::string> WorkbenchFactoryInst::workbenches() const
{
    std::list<std::string> wb;
    for (const auto& it : _mpcProducers) {
        wb.push_back(it.first);
    }
    return wb;
}
