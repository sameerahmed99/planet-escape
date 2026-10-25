#ifndef MEMORY_CORE
#define MEMORY_CORE


internal mi memory_get_page_size();
internal void *memory_reserve(mi size);
internal void *memory_reserve_at(mi size, void *location);
internal b32 memory_commit(void* mem, mi size);
internal b32 memory_decommit(void* mem, mi size);
internal b32 memory_free(void* mem, mi size);



#endif // MEMORY_CORE
