

typedef struct LogState{
  File file;
}LogState;

internal LogState g_internalLogState;



void _log_init(){
  g_internalLogState.file = file_open(str8_lit("logs.txt"), AccessFlag_Write);
}

void _log_impl(const char* fileName, i32 lineNum,const char* msg, ...){




  va_list args;


  va_start(args, msg);

  u8 bufferData[256];
  string8 buffer;
  buffer.ptr = bufferData;
  sprintf(bufferData, msg, args);



  sprintf(bufferData, "[%s:%d] ", fileName, lineNum);
  // null terminated length
  buffer.length = strlen(bufferData);
  printf("%s", bufferData);
  file_write(g_internalLogState.file, buffer);

  
  vsnprintf(bufferData, 256,msg, args);

  va_end(args);


  buffer.length = strlen(bufferData);

  printf("%s\n", bufferData);
  file_write(g_internalLogState.file, buffer);
  file_write(g_internalLogState.file, str8_lit("\n"));

}
