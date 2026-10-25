#include "arena.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: we want arena chaining so that we can free blocks in situations where it grows temporarily, such as when using an arena as temp arena to load something and then we don't need it anymore


internal Arena *arena_create(mi commitSize, mi reserveSize){

  mi cmt=0;
  mi res = 0;
  void *buffer;


  
  mi pageSize = memory_get_page_size();


  
  cmt = AlignPow2Multiple(commitSize, pageSize);
  res = AlignPow2Multiple(reserveSize, pageSize);

  buffer = memory_reserve(res);
  memory_commit(buffer, cmt);

  
  Arena *arena = (Arena*)buffer;

  arena->buffer = (u8*)buffer;
  arena->commitSize = cmt;
  arena->committed = cmt;
  arena->reserveSize = res;
  arena->reserved = res;
  arena->pos = ARENA_BASE_OFFSET;
  arena->pagesCommitted = 1;


  return arena;
}

internal void *arena_push(Arena *_arena, mi size,mi align, b32 zero){

  ///////////////////////////////////////////////////////////////////////////////////
  // ~sameer: no need to align the actual memory address, aligning the offset will do
  // When you do VirtualAlloc, you get a page size aligned memory address, which will be a power of 2 and a big number
  // such as 0b10000000 (128). When you pass an alignment value (which is also a power of 2) such as 0b1000 (8), which will always be a factor of the page size aligned memory address (base address of the arena buffer) and so as long as we ensure the local offset is aligned, the virtual mem address will also be aligned.
  // in other words, since the arena base mem address is a large power of 2 already, it will be aligned to any smaller power of 2s you want to align to
  // for eg in base ten, if page address is always power of 10 for eg 10000, it is divisible by any smaller power of 10 like 100.

  // sizeof(struct) will always be a multiple of a power of 2, alignof(struct) will always be a power of 2
  // sizeof will always be a multiple of alignof
  // which means we can walk the arena by using currentPos+sizeof(struct)
  // Alignment stuff is just for other manual, non struct pushes 
  Assert(IsPowerOf2(align));

  mi startPos = (AlignPow2Multiple(_arena->pos,align));
  // from mem address to local offset
  mi futurePos=startPos + size;
  


  mi amountToZero = futurePos - startPos;;
  if(futurePos > _arena->reserved){
    _log("Arena out of reserved memory, reduce memory usage or increase reserve size\n");
    Assert(False);
  }

  if(futurePos > _arena->committed){
    


    mi futurePosRelToNextPage = futurePos - _arena->pagesCommitted * _arena->commitSize;


    mi pagesToCommit = 1;

    if(futurePosRelToNextPage > _arena->commitSize){
      mi pagesAvailable = _arena->reserved - _arena->committed;
      pagesAvailable/=_arena->commitSize;

      if(pagesAvailable == 0){
	_log("no pages available\n");
      }
      Assert(pagesAvailable>0);
      
      pagesToCommit = futurePosRelToNextPage / _arena->commitSize;
      pagesToCommit +=1;

      pagesToCommit = Min(pagesAvailable, pagesToCommit);
    }
    mi cmtSize = pagesToCommit * _arena->commitSize;

    
    //u8 *commitLocation = (u8*)_arena->buffer + _arena->pos;
    u8 *commitLocation = (u8*)_arena->buffer + _arena->committed;
    memory_commit(commitLocation, cmtSize);

    _arena->committed+=cmtSize;
    _arena->pagesCommitted+=pagesToCommit;
  }


  
  void *ret = (void*)(_arena->buffer + startPos);
  if(zero && amountToZero>0){
    MemorySet(ret, 0, amountToZero);
  }
  _arena->pos = futurePos;
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

  mi prev = arena_pos(arena);
  pos = Max(ARENA_BASE_OFFSET, pos);

  // convert to local offset
  arena->pos = pos;

  mi prevPageNum = (prev / arena->commitSize);
  mi curPageNum = (pos /arena->commitSize);
  mi numCrossedPages =  prevPageNum - curPageNum;

  if(numCrossedPages > 0){
    mi startPage = curPageNum+1;
    mi freeSize = numCrossedPages * arena->commitSize;
    mi startPos = startPage * arena->commitSize;

    arena->committed -= freeSize;
    arena->pagesCommitted = curPageNum+1;
    memory_decommit(arena->buffer + startPos, freeSize);
  }


}
internal mi arena_pos(Arena *arena){
  return arena->pos;
}


internal u8 *arena_get(Arena *arena, mi globalPos){
  return arena->buffer + ARENA_BASE_OFFSET + globalPos;
}
internal TempArena arena_temp_begin(Arena *arena){
  TempArena ta = {arena, arena_pos(arena)};
  return ta;
  
}
internal void arena_temp_end(TempArena temp){
  arena_pop_to(temp.arena, temp.originalOffset);
}


internal mi arena_count_entries(Arena *arena, mi structSize){
  return (arena->pos - ARENA_BASE_OFFSET) / structSize;
}
