#include <unordered_map>

#include "BindingManager.h"

using namespace Base;

struct BindingManager::BindingManagerPrivate
{
    std::unordered_map<const void*, PyObject*> wrapperMapper;

    bool hasWrapper(const void* cptr)
    {
        auto it = wrapperMapper.find(cptr);
        return it != wrapperMapper.end();
    }

    void registerWrapper(const void* cptr, PyObject* pyObj)
    {
        wrapperMapper[cptr] = pyObj;
    }

    void releaseWrapper(const void* cptr, PyObject* pyObj)
    {
        auto it = wrapperMapper.find(cptr);
        if (it != wrapperMapper.end() && (!pyObj || it->second == pyObj)) {
            wrapperMapper.erase(it);
        }
    }

    PyObject* retrieveWrapper(const void* cptr)
    {
        auto it = wrapperMapper.find(cptr);
        if (it != wrapperMapper.end()) {
            return it->second;
        }

        return nullptr;
    }
};

BindingManager& BindingManager::instance()
{
    static BindingManager singleton;
    return singleton;
}

BindingManager::BindingManager()
    : p(new BindingManagerPrivate)
{}

BindingManager::~BindingManager() = default;

bool BindingManager::hasWrapper(const void* cptr)
{
    return p->hasWrapper(cptr);
}

void BindingManager::registerWrapper(const void* cptr, PyObject* pyObj)
{
    p->registerWrapper(cptr, pyObj);
}

void BindingManager::releaseWrapper(const void* cptr, PyObject* pyObj)
{
    p->releaseWrapper(cptr, pyObj);
}

PyObject* BindingManager::retrieveWrapper(const void* cptr)
{
    return p->retrieveWrapper(cptr);
}
