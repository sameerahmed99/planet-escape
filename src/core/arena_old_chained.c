#include "arena_old_chained.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: we want arena chaining so that we can free blocks in situations where it grows temporarily, such as when using an arena as temp arena to load something and then we don't need it anymore


internal Arena *arena_create(mi commitSize, mi reserveSize, u8 *optionalBackBuffer){

  mi cmt=0;
  mi res = 0;
  void *buffer;
  if(optionalBackBuffer!=0){
    buffer = (void*)optionalBackBuffer;
  }
  else{
    mi pageSize = memory_get_page_size();
    cmt = AlignPow2Multiple(commitSize, pageSize);
    res = AlignPow2Multiple(reserveSize, pageSize);

    buffer = memory_reserve(res);
    memory_commit(buffer, cmt);
  }
  
  Arena *arena = (Arena*)buffer;

  arena->current = arena;
  arena->buffer = (u8*)buffer;
  arena->commitSize = cmt;
  arena->committed = cmt;
  arena->reserveSize = res;
  arena->reserved = res;
  arena->offset = ARENA_BASE_OFFSET;
  arena->globalOffset = 0;

  return arena;
}

internal void *arena_push(Arena *_arena, mi size,mi align, b32 zero){

  ///////////////////////////////////////////////////////////////////////////////////
  // ~sameer: no need to align the actual memory address, aligning the offset will do
  // When you do VirtualAlloc, you get a page size aligned memory address, which will be a power of 2 and a big number
  // such as 0b10000000 (128). When you pass an alignment value (which is also a power of 2) such as 0b1000 (8), which will always be a factor of the page size aligned memory address (base address of the arena buffer) and so as long as we ensure the local offset is aligned, the virtual mem address will also be aligned.
  // in other words, since the arena base mem address is a large power of 2 already, it will be aligned to any smaller power of 2s you want to align to
  // for eg in base ten, if page address is always power of 10 for eg 10000, it is divisible by any smaller power of 10 like 100.

  // sizeof(some struct) will always be a multiple of a power of 2, alignof(some struct) will always be a power of 2
  // sizeof will always be a multiple of alignof
  // which means we can walk the arena by using currentPos+sizeof(struct)
  Assert(IsPowerOf2(align));
  Arena *currentArena = _arena->current;
  mi startPos = (AlignPow2Multiple(currentArena->offset,align));

  // from mem address to local offset

  mi futurePos=startPos + size;
  


  mi amountToZero = futurePos - startPos;;
  if(futurePos > currentArena->reserved){
    mi resSize = currentArena->reserveSize;
    mi cmtSize = currentArena->commitSize;

    if(size+ARENA_BASE_OFFSET > resSize){
      resSize = AlignPow2Multiple(size+ARENA_BASE_OFFSET, align);
      cmtSize = AlignPow2Multiple(size+ARENA_BASE_OFFSET, align);
    }
    Arena *new = arena_create(cmtSize,resSize,0);
    new->globalOffset = currentArena->reserved;

    _arena->current = new;
    currentArena->next = new;
    _arena->current->prev = currentArena;

    currentArena = new;

    startPos = AlignPow2Multiple(currentArena->offset, align);
    futurePos = startPos + size;
    amountToZero = 0;

  }

  if(futurePos > currentArena->committed){
    
    // align up to the arena' commit size, same logic as AlignPow2Multiple. We push it above the next aligned amount, then we subtract the extra. eg  5 + 999 = 1004. In this example 5 is the size of the commit we're trying to do, 1000 is arena' cmtSize. We have 4 extra now, so 1004 % 1000 = 4 i.e we now sutbract 4 from 1004
    mi cmtSize = futurePos + currentArena->commitSize - 1;
    cmtSize -= cmtSize % currentArena->commitSize;

    mi maxAvailableToCommit = currentArena->reserved - currentArena->committed;
    cmtSize = Min(maxAvailableToCommit, cmtSize);
    
    u8 *commitLocation = (u8*)currentArena->buffer + currentArena->offset;
    memory_commit(commitLocation, cmtSize);

    currentArena->committed+=cmtSize;
  }


  
  void *ret = (void*)(currentArena->buffer + currentArena->offset);
  if(zero && amountToZero>0){
    MemorySet(ret, 0, amountToZero);
  }
  currentArena->offset = futurePos;
  return ret;
}
internal void arena_clear(Arena *arena){
  arena_pop_to(arena,0);
}
internal void arena_pop(Arena *arena, mi amount){
  mi cur = arena_pos(arena);
  if(amount>cur) return;
  
  mi popLocation = cur- amount;
  arena_pop_to(arena, popLocation);
}
internal void arena_pop_to(Arena *arena, mi pos){

  pos = Max(ARENA_BASE_OFFSET, pos);
  
  while(pos < arena->current->globalOffset){
    Arena *newCurrent = arena->current->prev;

    memory_free(arena, arena->current->reserved);

    newCurrent->next = 0;
    arena->current = newCurrent;
  }

  // convert to local offset
  arena->current->offset = pos - arena->current->globalOffset;


}
internal mi arena_pos(Arena *arena){
  return arena->current->globalOffset + arena->current->offset;
}


//////////////////////
// ~sameer: @DANGEROUS
// it doesn't check if you're near the boundary and if half of the struct or whatever you cast it to
// will bleed into the next chain block
internal u8 *arena_get(Arena *arena, mi globalPos){
  globalPos+=ARENA_BASE_OFFSET;
  u32 arenaIndex = globalPos / arena->reserveSize;
  u32 local = globalPos % arena->reserveSize;

  int block = 0;
  Arena *a = arena;
  while(block != arenaIndex){
    a=a->next;
    block++;
  }
  return a->buffer + local;
}
internal TempArena arena_temp_begin(Arena *arena){
  TempArena ta = {arena, arena_pos(arena)};
  return ta;
  
}
internal void arena_temp_end(TempArena temp){
  arena_pop_to(temp.arena, temp.originalOffset);
}


