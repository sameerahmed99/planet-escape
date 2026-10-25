#include "file.h"



internal string8 file_read_from_path_into_buffer(Arena *arena, string8 path){
  File file = file_open(path, AccessFlag_Read | AccessFlag_ShareRead);

  FileProperties props = file_properties_from_file(file);
  string8 res = file_read_range_to_string8(arena, file, range_mi(0, props.size));
  file_close(file);
  return res;
}

internal string8 file_read_range_to_string8(Arena *arena, File file, Range_mi range){
  string8 res = {0};

  mi prevPos = arena_pos(arena);

  u64 desiredReadSize = range_mi_length(range);
  res.ptr = PushArrayNoZero(arena, u8, desiredReadSize);
  res.length = desiredReadSize;

  mi bytesRead = file_read(file, range, (void*)res.ptr);
  if(bytesRead<res.length){
    arena_pop_to(arena, prevPos + bytesRead);
    res.length = bytesRead;
  }
  return res;
}
File file_get_empty(){
  File f = {0};

  return f;
}

internal b32 file_match(File a,File b){
  b32 res = MemoryIsMatchStruct(&a, &b);
  return res;
}
internal i64 file_id_compare(FileID a, FileID b){
  i64 res = MemoryCompare((void*)a.v[0],(void*)b.v[0], sizeof(a.v));
  return res;
}

