#ifndef ARENA_CORE_INCLUDE
#define ARENA_CORE_INCLUDE

#include "core.h"






typedef struct Arena Arena;
struct Arena{
  Arena *current;
  Arena *prev;
  Arena *next;
  u8 *buffer;
  mi globalOffset;
  mi reserved;
  mi committed;
  mi offset;
  mi commitSize;
  mi reserveSize;
};


typedef struct TempArena TempArena;

struct TempArena{
  Arena *arena;
  mi originalOffset;
};

#define ARENA_BASE_OFFSET 128

StaticAssert(ARENA_BASE_OFFSET > sizeof(Arena), arena_header_offset_check);

////////////////////////////////
// ~sameer: arena core functions

internal Arena *arena_create(mi commitSize, mi reserveSize, u8 *optionalBackBuffer);
internal void *arena_push(Arena *arena, mi size,mi align, b32 zero);
internal void arena_clear(Arena *arena);
internal void arena_pop(Arena *arena, mi amount);
internal void arena_pop_to(Arena *arena, mi pos);
internal mi arena_pos(Arena *arena);
internal u8 *arena_get(Arena *arena, mi globalPos);
internal TempArena arena_temp_begin(Arena *arena);
internal void arena_temp_end(TempArena temp);



////////////////////////
// ~sameer: arena macros
#define PushStruct(arena, T) (T*)arena_push(arena, sizeof(T), AlignOf(T), True)
#define PushStructNoZero(arena, T) (T*)arena_push(arena, sizeof(T), AlignOf(T), False)
#define PushArray(arena, T, count) (T*)arena_push(arena, sizeof(T)*(count), AlignOf(T), True)
#define PushArrayNoZero(arena, T, count) (T*)arena_push(arena, sizeof(T)*(count), AlignOf(T),False)


#define PopStruct(arena, T) arena_pop(arena, sizeof(T))
#define PopArray(arena, T, count) arena_pop(arena, sizeof(T)*(count))

#define GetStruct(arena, pos, T) (T*)arena_get(arena, (pos), T)


#endif // ARENA_CORE_INCLUDE
