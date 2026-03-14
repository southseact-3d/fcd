#ifndef BASE_BINDINGMANAGER_H
#define BASE_BINDINGMANAGER_H

#include <memory>
#include <FCGlobal.h>

using PyObject = struct _object;

namespace Base
{

class BaseExport BindingManager
{
public:
    static BindingManager& instance();

    bool hasWrapper(const void* cptr);

    void registerWrapper(const void* cptr, PyObject* pyObj);
    void releaseWrapper(const void* cptr, PyObject* pyObj);

    PyObject* retrieveWrapper(const void* cptr);

    BindingManager(const BindingManager&) = delete;
    BindingManager(BindingManager&&) = delete;
    BindingManager& operator=(const BindingManager&) = delete;
    BindingManager& operator=(BindingManager&&) = delete;

private:
    BindingManager();
    ~BindingManager();

    struct BindingManagerPrivate;
    std::unique_ptr<BindingManagerPrivate> p;
};

}  // namespace Base

#endif  // BASE_BINDINGMANAGER_H
