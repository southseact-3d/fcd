#ifndef ERROR_STACK_H
#define ERROR_STACK_H

#include "utility.h"
#include "utility_io.h"
#include <string>


namespace KDL {

/*
 * \todo
 *    IOTrace-routines store in static memory, should be in thread-local memory.
 * pushes a description of the current routine on the IO-stack trace
 */
void IOTrace(const std::string& description);

//! pops a description of the IO-stack
void IOTracePop();


//! outputs the IO-stack to a stream to provide a better errormessage.
void IOTraceOutput(std::ostream& os);

//! outputs one element of the IO-stack to the buffer (maximally size chars)
//! returns empty string if no elements on the stack.
void IOTracePopStr(char* buffer,int size);


}

#endif

