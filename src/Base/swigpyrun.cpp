#include <sstream>
#if (defined(HAVE_SWIG) && (HAVE_SWIG == 1))
# if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
# elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#  pragma GCC diagnostic ignored "-Wcast-function-type"
# endif
# include "PyExport.h"
# include "Exception.h"
namespace Swig_python
{
# define SWIG_PYTHON_NO_BUILD_NONE
# include <swigpyrun.h>
# include "swigpyrun.inl"
}  // namespace Swig_python
# if defined(__clang__)
#  pragma clang diagnostic pop
# elif defined(__GNUC__)
#  pragma GCC diagnostic pop
# endif
#endif  // HAVE_SWIG
