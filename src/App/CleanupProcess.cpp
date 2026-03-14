#include <list>

#include "CleanupProcess.h"


using namespace App;

namespace
{
static std::list<std::function<void()>> cleanup_funcs;  // NOLINT
}

void CleanupProcess::registerCleanup(const std::function<void()>& func)
{
    cleanup_funcs.push_back(func);
}

void CleanupProcess::callCleanup()
{
    for (const auto& func : cleanup_funcs) {
        func();
    }
}
