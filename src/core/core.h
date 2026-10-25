#ifndef CORE_INCLUDE
#define CORE_INCLUDE

#include <stdint.h>
#include <string.h>
#include "platform_detection.h"



/////////////////////////////////
// ~sameer: basic number typedefs


typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef i32 b32;

typedef float r32;
typedef double r64;



//////////////////////////////////////////////////////////////////////////////
// ~sameer: mi is for memory index,
// uptr is for holding pointer values as integers to do math on and whatnot. If you use void* instead of uptr then you have to cast void* to something like u8* to do math on
typedef size_t mi;
typedef uintptr_t uptr;


const b32 False = 0;
const b32 True = 1;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: still not entirely sure why handmade hero and raddbg use internal/global for variables and functions in a unity build
// the closest I've gotten to an answer is that maybe the binary is smaller or the compiler is more likely to inline a function if it's not expected to be used by another translation unit, stick to using it just in case but find out more.

#define internal static
#define global static
#define local_persist static

////////////////
// ~sameer: misc

// we do this because if we use ConcatCode_ direction, and we pass (codeA, __LINE__) then it will give us code__LINE__ without evaluating __LINE__
// but if we redirect it from another macro, it will have a chance to evaluate into the actual line number
#define ConcatCode_(A,B) A##B
#define ConcatCode(A,B) ConcatCode_(A,B)


/////////////////////////
// ~sameer: assert macros

#if BUILD_DEBUG
#define Assert(Expression) do { if(!(Expression)){*(int*)0=0;} } while(0)
#else

#define Assert(Expression) (void)(Expression)
#endif

// create a u8 array of size 1 if all good, and of size -1 to cause compilation failure if condition false
// the id and line thing is to give the variable a unique name. Id is for your convinience, __LINE__ is to make it unique, for eg if you're doing multiple asserts and want to use the same id
#define StaticAssert(condition,id) global u8 ConcatCode(id, __LINE__)[(condition) ? 1 : -1]

/////////////////
// ~sameer: units


#define KB(Value) ((mi)(Value)*1024)
#define MB(Value) (KB((mi)(Value))*1024)
#define GB(Value) (MB((mi)(Value))*1024)
#define TB(Value) (GB((mi)(Value))*1024)
#define Thousand(v) ((v)*1000)
#define Million(v) (Thousand(v)*Thousand(v))
#define Billion(v) (Million(v)*Thousand(v))


///////////////////////
// ~sameer: math macros


#define Swap(T, A, B) do {T t = A; A = B; B = t;} while (0)
#define Min(A, B) ((A) < (B) ? (A) : (B))
#define Max(A, B) ((A) > (B) ? (A) : (B))
#define Clamp01(A) Max(0, Min(1, (A)))
#define Clamp(val, min, max) Min( (max), Max( (min), (val) ))
#define Sign(A) (((A) > 0) - ((A) < 0))
#define Abs(A) (Sign(A)*(A))
#define Lerp(A,B,time) ( (A) + ((B)-(A))*(time))
#define Rand01() ((rand() % 100)/100.0f)
#define Rand11() ((Rand01()-0.5f)*2.0f)


//////////////////
// ~sameer: memory
#define MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define MemorySet(dst, byte, size)    memset((dst), (byte), (size))
#define MemoryCompare(a, b, size)     memcmp((a), (b), (size))

#define MemoryIsMatch(a,b,size)      (MemoryCompare((a),(b),(size)) == 0)
#define MemoryIsMatchStruct(a,b)     (MemoryCompare((a),(b),(sizeof(*a))) == 0)

////////////////////////
// ~sameer: access flags
typedef u32 AccessFlags;
enum
{
  AccessFlag_Read        = (1<<0),
  AccessFlag_Write       = (1<<1),
  AccessFlag_Execute     = (1<<2),
  AccessFlag_Append      = (1<<3),
  AccessFlag_ShareRead   = (1<<4),
  AccessFlag_ShareWrite  = (1<<5),
  AccessFlag_Inherited   = (1<<6),

};

////////////////////////////
// ~sameer: alignment functions/macros


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: a very cool realization from this AlignPow2Multiple macro: let's think about base 10 number system first
// let's take powers of ten: 10, 100, 1000 etc
// how can you tell at a glance if a number is divisible by one of these powers?
// e.g, how do you know if a number is divisible by 1000 at a glance?
// simply by checking if the last 3 digits (bottom 3) are zero or not (e.g, 5232000, 21000, 213541000)
// anything else, and it's not divisible by 1000.

// same is true for base 2,
// e.g, you can tell that 11101100 is divisible by 00000100 (4 in base 10) by checking if the number of zeros at the end is the same in both numbers.

// so the whole goal of this macro is to move the number to the next multiple (or bigger) of whatever power of 2 you are aligning to
// and then clearing out the bottom bits
// none of the higher bits matter, just as long as the <number of zeros in the power of two you are using> are zero.
// for eg, if aligning to 8 (00001000) you will clear out the bottom 3 bits. Bits 4 and above are irrelevant, they need to be preserved.
// that's what the inversion is for: 8 - 1 = 7 = (00000111) and then invert it to turn on all bits except for the bottom 3. 

// number system is irrelevant, this trick works as long as we're alinging to a power of the number of symbols in number system, because all such powers have only 1 bit turned on and the rest are off

// another way to think about this is to just think of the numbers as you normally would in real life instead of binary number
// 101000 is a binary number, but think of it as one hundred one thousand. This number is only divisible by 1 thousand (still in binary) if there are three zeros at the end, same as in base 10. The number system is irrelevant, numbers ending with three zeros in any number system are factors of 1 thousand, regardless of what quantity the number 1 thousand represents. Obvious in retrospect but this helped my slow math brain finally click

// a few more examples to think about:
// the base 10 number 2150
// is saying that it contains two thousands in it as well as a bit more additional quanity (150)
// that's why it's not fully divisible by 1000, the extra quantity has to go either by rounding down to 2000 or going up to 3000

// the base 2 number 10111001 is saying that it has 10111 eights in it (1000 aka one thousand in binary) and a bit more additional quantity (1001)
// so it's not fully divisible by 8 unless we round up or round down such it becomes a full multiple of eight (1000 aka one thousand in binary)


#define AlignPow2Multiple(x,b)     (((x) + (b) - 1)&(~((b) - 1)))

#if COMPILER_GCC
# define AlignOf(T) __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

#define IsPowerOf2(x) (((x) & ((x)-1)) == 0)



// file name and line number macors
#if COMPILER_GCC
#define _COMPILER_FILE_NAME_ __FILE_NAME__
#define _COMPILER_LINE_NUM_ __LINE__
#else
#error undetected compiler while setting up file and line number macros
#endif

#endif
