#pragma once

#include <set>
#include <string>
#include <vector>

// Define the following symbol adds some functions to the API for implementation
// purposes. These functions are not available to a normal API user.
#define E57_INTERNAL_IMPLEMENTATION_ENABLE 1
#include "E57Format.h"

#ifdef _MSC_VER
// Disable MSVC warning: warning C4224: nonstandard extension used : formal
// parameter 'locale' was previously defined as a type
#pragma warning( disable : 4224 )
#endif

// Used to mark unused parameters to indicate intent and suppress warnings.
#define E57_UNUSED( expr ) (void)( expr )

// For readability of preprocessor using E57_VALIDATION_LEVEL
#define VALIDATION_OFF 0
#define VALIDATION_BASIC 1
#define VALIDATION_DEEP 2

#define VALIDATE_BASIC ( E57_VALIDATION_LEVEL > VALIDATION_OFF )
#define VALIDATE_DEEP ( E57_VALIDATION_LEVEL > VALIDATION_BASIC )

// Determine if we are building 32 or 64 bit
#if SIZE_MAX == UINT32_MAX
#define E57_32_BIT
#elif SIZE_MAX == UINT64_MAX
#define E57_64_BIT
#endif

namespace e57
{
#define E57_EXCEPTION1( ecode )                                                                    \
   ( E57Exception( ( ecode ), ustring(), __FILE__, __LINE__,                                       \
                   static_cast<const char *>( __FUNCTION__ ) ) )
#define E57_EXCEPTION2( ecode, context )                                                           \
   ( E57Exception( ( ecode ), ( context ), __FILE__, __LINE__,                                     \
                   static_cast<const char *>( __FUNCTION__ ) ) )

   using ImageFileImplSharedPtr = std::shared_ptr<class ImageFileImpl>;
   using ImageFileImplWeakPtr = std::weak_ptr<class ImageFileImpl>;
   using NodeImplSharedPtr = std::shared_ptr<class NodeImpl>;
   using NodeImplWeakPtr = std::weak_ptr<class NodeImpl>;

   using StringList = std::vector<std::string>;
   using StringSet = std::set<std::string>;

   /// generates a new random GUID
   std::string generateRandomGUID();
}
