#include "Observer.h"


namespace Base
{

#if !defined(__MINGW32__)
template class BaseExport Observer<const char*>;
template class BaseExport Subject<const char*>;
#endif

}  // namespace Base
