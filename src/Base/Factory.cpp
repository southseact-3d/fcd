#include <list>

#include "Factory.h"
#include "Console.h"

using namespace Base;


Factory::~Factory()
{
    for (auto& it : _mpcProducers) {
        delete it.second;
    }
}

void* Factory::Produce(const char* sClassName) const
{
    std::map<const std::string, AbstractProducer*>::const_iterator pProd;

    pProd = _mpcProducers.find(sClassName);
    if (pProd != _mpcProducers.end()) {
        return pProd->second->Produce();
    }

    return nullptr;
}

void Factory::AddProducer(const char* sClassName, AbstractProducer* pcProducer)
{
    _mpcProducers[sClassName] = pcProducer;
}

bool Factory::CanProduce(const char* sClassName) const
{
    return (_mpcProducers.find(sClassName) != _mpcProducers.end());
}

std::list<std::string> Factory::CanProduce() const
{
    std::list<std::string> lObjects;

    for (const auto& it : _mpcProducers) {
        lObjects.push_back(it.first);
    }

    return lObjects;
}

// ----------------------------------------------------

ScriptFactorySingleton* ScriptFactorySingleton::_pcSingleton = nullptr;  // NOLINT


ScriptFactorySingleton& ScriptFactorySingleton::Instance()
{
    if (!_pcSingleton) {
        _pcSingleton = new ScriptFactorySingleton;
    }
    return *_pcSingleton;
}

void ScriptFactorySingleton::Destruct()
{
    delete _pcSingleton;
    _pcSingleton = nullptr;
}

const char* ScriptFactorySingleton::ProduceScript(const char* sScriptName) const
{
    const char* script = static_cast<const char*>(Produce(sScriptName));

    if (!script) {
#ifdef FC_DEBUG
        Console().warning("\"%s\" is not registered\n", sScriptName);
#endif
        return "";  // no data
    }

    return script;
}
