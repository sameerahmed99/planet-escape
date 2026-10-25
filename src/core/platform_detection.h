#ifndef PLATFORM_DETECTION_INCLUDE
#define PLATFORM_DETECTION_INCLUDE
# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
# else
#  error Whot de fok this os
# endif

// copied from rdbg
# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif


#if defined(__GNUC__) || defined(__GNUG__)

# define COMPILER_GCC 1
#else
# error Compiler unsupported
#endif // COMPILER_DETECTION

#endif // PLATFORM_DETECTION_INCLUDE
