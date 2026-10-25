#ifndef FILE_SYS_INCLUDE
#define FILE_SYS_INCLUDE


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: I believe raddbg uses these kinds of ids to allow the platform to decide how to use the array in the struct. Some might have a memory address, some might have some handle number etc. The size of the array should be based on the platform which has the biggest id. Change this later if it turns out to be unecessary for the game.


// the file id can be used to compare files, see "BY_HANDLE_FILE_INFORMATION"  for win32.
// get this handle and then store the info in the v3 to compare later

typedef struct FileID FileID;
struct FileID
{
  u64 v[3];
};


typedef struct File File;
struct File
{
  u64 v[1];
} ;

// keep this separate from File struct as file properties can change while the file is open, code that needs props can get fresh ones
typedef struct FileProperties FileProperties;
struct FileProperties{
  u64 size;
};


internal string8 file_read_from_path_into_buffer(Arena *arena, string8 path);
internal i64 file_id_compare(FileID a, FileID b);
internal File file_get_empty();

internal b32 file_match(File a,File b);
internal string8 file_read_range_to_string8(Arena *arena, File file, Range_mi range);

///////////////////////////////////////////////////////
// ~sameer: functions that need to be implemented for each os

internal File file_open(string8 path, AccessFlags flags);
internal void file_close(File file);
internal mi file_read(File file, Range_mi range, void *outData);
internal b32 file_write(File file, string8 data);

internal FileID file_id_from_file(File file);
internal FileProperties file_properties_from_file(File file);

#endif // FILE_SYS_INCLUDE
