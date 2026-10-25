#ifndef LOG_CORE_INCLUDE
#define LOG_CORE_INCLUDE
#include <stdio.h>






void _log_init();
// underscore to avoid name conflict 
void _log_impl(const char* fileName, i32 lineNumber, const char* msg, ...);




// the double hash allows us to do optiona VA_ARGS
#define _log(msg, ...) _log_impl(_COMPILER_FILE_NAME_, _COMPILER_LINE_NUM_, msg, ##__VA_ARGS__)




#endif // LOG_CORE_INCLUDE
