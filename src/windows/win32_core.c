#include "win32_core.h"
#include "win32_audio.h"
#include "win32_audio.c"
#include <GL/GL.h>







global RECT Win32WindowRect;
global HWND Win32Window;


typedef void (*OSRecursiveDirectoryIterationCallback)(i32, const char*, const char*);






HWND win32_get_window(){
  return Win32Window;
}





internal void set_input_state(InputState *input, b32 wasDownedThisFrame, b32 isPressed, b32 wasReleasedThisFrame);
internal void win32_reset_input_state(InputState *input, b32 preserveIsPressed);



internal i32 recursively_read_files_in_directory(const char* path, OSRecursiveDirectoryIterationCallback callback);


internal void set_input_state(InputState *input, b32 wasDownedThisFrame, b32 isPressed, b32 wasReleasedThisFrame){
  input->wasDownedThisFrame = wasDownedThisFrame;
  input->isPressed = isPressed;
  input->wasReleasedThisFrame = wasReleasedThisFrame;
}
internal void win32_reset_input_state(InputState *input, b32 preserveIsPressed){



  set_input_state(input, False, (preserveIsPressed && input->isPressed), False);
  
}







internal RECT win32_get_window_rect(HWND _window, mi *width, mi *height){
  RECT rect;
  GetClientRect(_window, &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
  return rect;
}
internal void win32_resize_window(AppState *app, mi width, mi height) {
  app->screenWidth = width;
  app->screenHeight = height;
  app->halfScreenWidth = width/2.0f;
  app->halfScreenHeight = height/2.0f;
}
internal void win32_resize_screen_buffer(Win32ScreenBuffer *buffer, mi _width, mi _height) {

  if (buffer->data) {
    VirtualFree(buffer->data, 0, MEM_RELEASE);
  }

  buffer->width = _width;
  buffer->height = _height;
  buffer->stride = 4;


  /* //@NOTE we set the bitmap height to negative buffer.height */
  // check the copy_buffer function, a new bitmapinfo struct was being created there and i swapped it with this
  BITMAPINFO Info;
  Info.bmiHeader.biSize = sizeof(Info);
  Info.bmiHeader.biWidth = buffer->width;
  Info.bmiHeader.biHeight = -buffer->height;
  Info.bmiHeader.biPlanes = 1;
  Info.bmiHeader.biBitCount = buffer->stride * 8;
  Info.bmiHeader.biCompression = BI_RGB;

  buffer->bitmapInfo = Info;
 

  buffer->data = VirtualAlloc(0, _width * _height * buffer->stride, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

}

internal LRESULT win32_callback(HWND _window, UINT _msgId, WPARAM param3, LPARAM param4) {

  AppState *app = &WinState.app;
  LRESULT result = 0;

  switch (_msgId) {

  case WM_CREATE:{

  } break;
  case WM_CLOSE: {
    OutputDebugStringW(L"VM_CLOSE\n");
    WinState.running = False;

  } break;
  case WM_DESTROY: {
    OutputDebugStringW(L"VM_DESTROY\n");
    WinState.running = False;
  }
  case WM_SIZE: {
    
  } break;
    /* case WM_RBUTTONDOWN: */
    /* case WM_MBUTTONDOWN: */
    /* case WM_XBUTTONDOWN: */
    /* case WM_LBUTTONUP: */
    /* case WM_RBUTTONUP: */
    /* case WM_MBUTTONUP: */
    /* case WM_XBUTTONUP: */

  case WM_LBUTTONDOWN:
    {
      b32 wasDownedThisFrame = !app->input.mouse1.isPressed;
      set_input_state(&app->input.mouse1,wasDownedThisFrame, True, False);
    } break;
  case WM_LBUTTONUP:
    {
      b32 wasReleasedThisFrame = app->input.mouse1.isPressed;
      set_input_state(&app->input.mouse1,False, False, wasReleasedThisFrame);
    } break;
  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP: {
    b32 wasDown = ((param4 & (1 << 30)) != 0);
    b32 isDown = ((param4 & (1 << 31)) == 0);
    b32 wasDownedThisFrame = !wasDown && isDown;
    b32 wasReleasedThisFrame = !isDown && wasDown;

    u32 code = param3;

    switch (code) {
    case 'W': {

      //	_log("W was down %d, is down %d\n", wasDown, isDown);
      set_input_state(&app->input.w, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case 'S': {
      set_input_state(&app->input.s, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;

    case 'A': {
      //	_log("W was down %d, is down %d\n", wasDown, isDown);
      set_input_state(&app->input.a, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case 'D': {
      set_input_state(&app->input.d, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case 'Q': {
      set_input_state(&app->input.q, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case 'E': {
      set_input_state(&app->input.e, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case VK_ESCAPE: {
      set_input_state(&app->input.escape, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case VK_SPACE:{
      set_input_state(&app->input.space, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case VK_MENU:{
      set_input_state(&app->input.lalt, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case VK_SHIFT:{
      set_input_state(&app->input.lshift, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    } break;
    case VK_LEFT:{
      set_input_state(&app->input.leftArrow, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    }break;
    case VK_RIGHT:{
      set_input_state(&app->input.rightArrow, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    }break;
    case VK_UP:{
      set_input_state(&app->input.upArrow, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    }break;
    case VK_DOWN:{
      set_input_state(&app->input.downArrow, wasDownedThisFrame, isDown, wasReleasedThisFrame);
    }break;
    case VK_F4: {
      b32 alting = (param4 & (1 << 29)) != 0;
      if (alting) {
        WinState.running = False;
      }
    } break;
    default: {

    } break;
    }
    default: {
      result = DefWindowProc(_window, _msgId, param3, param4);
    };
  };


  }
  return result;
}

internal void copy_buffer_to_window(HDC dc){


  Win32ScreenBuffer buffer = WinState.screenBuffer;



  /* BITMAPINFO Info; */
  /* Info.bmiHeader.biSize = sizeof(Info); */
  /* Info.bmiHeader.biWidth = buffer.width; */

  /* //@NOTE we set the bitmap height to negative buffer.height */
  /* // because windows stores the buffer upside down relative to what we expect */
  /* // we expect top left to be origin, like in a png */
  /* Info.bmiHeader.biHeight = -buffer.height; */
  /* Info.bmiHeader.biPlanes = 1; */
  /* Info.bmiHeader.biBitCount = buffer.stride * 8; */
  /* Info.bmiHeader.biCompression = BI_RGB; */
  StretchDIBits(dc, 0,0,
	        WinState.app.screenWidth,
	        WinState.app.screenHeight,
		0,0, buffer.width, buffer.height,
		buffer.data, &buffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

}

// COM
internal void win32_init_com_api() {


  HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

  if (FAILED(hr)) {
    _log("Couldn't initial COM\n");
    return;
  };
  _log("Initialized COM \n");
}



i32 WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, i32 nCmdShow) {
  _log_init();
  _log("Win32::Starting..\n");
  DesiredAppSettings desiredSettings = app_get_desired_settings();
  
  QueryPerformanceFrequency(&WinState.performanceQueryFrequency);
  WNDCLASSW windclass = {0};


  windclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windclass.lpfnWndProc = win32_callback;
  windclass.hInstance = hInstance;
  // windowclass.hIcon=;
  windclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  //  LPCWSTR   lpszMenuName;
  windclass.lpszClassName = L"Planet Escape";

  RegisterClassW(&windclass);

  i32 desWidth = desiredSettings.screenWidth;
  i32 desHeight = desiredSettings.screenHeight;
  if(desiredSettings.fullScreen){
    desWidth =GetSystemMetrics(SM_CXSCREEN);
    desHeight =GetSystemMetrics(SM_CYSCREEN);
  }

  _log("Win32::Screen height/width: %d/%d\n", desWidth, desHeight);
  HWND hwnd = CreateWindowExW(0, // Optional window styles.

                              windclass.lpszClassName,          // Window class
                              L"Planet Escape",                         // Window text
			      WS_SYSMENU |
                              WS_OVERLAPPED | WS_VISIBLE | WS_POPUP, // Window style
			      

                              // Size and position
                              CW_USEDEFAULT, CW_USEDEFAULT,
			      desWidth,
			      desHeight,

                              NULL,      // Parent window
                              NULL,      // Menu
                              hInstance, // Instance handle
                              NULL       // Additional application data
  );
  Win32Window = hwnd;

  if (!hwnd) {
    OutputDebugStringW(L"Couldn't create window\n");
    _log("Win32::Couldn't create window\n");
    return -1;
  }
  _log("Win32::Created window\n");

  // INIT COM
  win32_init_com_api();

  //////

  /////////

  

  // INIT INPUT
  win32_reset_input_state(&WinState.app.input.w, False);
  win32_reset_input_state(&WinState.app.input.a, False);
  win32_reset_input_state(&WinState.app.input.s, False);
  win32_reset_input_state(&WinState.app.input.d, False);
  win32_reset_input_state(&WinState.app.input.space, False);
  win32_reset_input_state(&WinState.app.input.q, False);
  win32_reset_input_state(&WinState.app.input.e, False);
  win32_reset_input_state(&WinState.app.input.lshift, False);
  win32_reset_input_state(&WinState.app.input.lalt, False);
  win32_reset_input_state(&WinState.app.input.escape, False); 
  win32_reset_input_state(&WinState.app.input.mouse1, False);
  win32_reset_input_state(&WinState.app.input.mouse2, False);
  win32_reset_input_state(&WinState.app.input.mouse3, False);
  win32_reset_input_state(&WinState.app.input.leftArrow, False);
  win32_reset_input_state(&WinState.app.input.upArrow, False);
  win32_reset_input_state(&WinState.app.input.rightArrow, False);
  win32_reset_input_state(&WinState.app.input.downArrow, False);
  
  _log("Win32::Init input\n");
  WinState.app.renderScale = desiredSettings.renderScale;


  mi winWidth, winHeight;
  Win32WindowRect = win32_get_window_rect(hwnd, &winWidth, &winHeight);
  win32_resize_window(&WinState.app, winWidth, winHeight);
  win32_resize_screen_buffer(&WinState.screenBuffer, winWidth*WinState.app.renderScale, winHeight*WinState.app.renderScale);

  
  WinState.running = True;



 
  win32_init_wasapi(desiredSettings, &WinState);

  


  _log("initializing user app");
  Window windowHandle = { {(u64)hwnd} };
  app_init(&WinState.app, windowHandle);
  _log("Finished initializing user app");
  MSG msg = {0};



  LARGE_INTEGER previousFrameTimeStamp;
  QueryPerformanceCounter(&previousFrameTimeStamp);

  _log("Win32::Initialization complete, beginning main loop\n");

  b32 firstLoop = True;
  b32 firstUpdate = True;
  b32 firstVideoUpdate = True;
  b32 warmingUp = True;

  // SwapBuffers causes some delay at the start. Skip a few frames, keep dt 0 until then
  i32 warmupFrames = 5;
  i32 currWarmupFrame = 0;


  while(WinState.running){
    if(firstLoop){
      _log("First loop start\n");
    }

    
    LARGE_INTEGER perfTimeStamp;
    QueryPerformanceCounter(&perfTimeStamp);
    i64 countsSinceLastFrame = perfTimeStamp.QuadPart - previousFrameTimeStamp.QuadPart;

    r64 secPerCount = 1.0/WinState.performanceQueryFrequency.QuadPart;
    r64 secondsSinceLastFrame = (r64)countsSinceLastFrame*secPerCount;
    r32 dt = (r32)secondsSinceLastFrame;


    r32 targetFrameTime = 1/60.0f;
    b32 timeCorrection=dt<targetFrameTime;
    r32 timeCorrectionAmount = targetFrameTime - dt;
    while(timeCorrection){
      LARGE_INTEGER curr;
      QueryPerformanceCounter(&curr);
      r32 secs = (curr.QuadPart-perfTimeStamp.QuadPart)*secPerCount;
      timeCorrection = secs <timeCorrectionAmount;
      dt = targetFrameTime;
    }

    QueryPerformanceCounter(&previousFrameTimeStamp);

    if(currWarmupFrame < warmupFrames){
      dt = 0;
      currWarmupFrame++;
      if(currWarmupFrame<warmupFrames){
	_log("Warming up");
      }
      else{
	_log("Finished warming up");
      }
      
    }




    win32_reset_input_state(&WinState.app.input.w, True);
    win32_reset_input_state(&WinState.app.input.a, True);
    win32_reset_input_state(&WinState.app.input.s, True);
    win32_reset_input_state(&WinState.app.input.d, True);
    win32_reset_input_state(&WinState.app.input.space, True);
    win32_reset_input_state(&WinState.app.input.q, True);
    win32_reset_input_state(&WinState.app.input.e, True);
    win32_reset_input_state(&WinState.app.input.lshift, True);
    win32_reset_input_state(&WinState.app.input.lalt, True);
    win32_reset_input_state(&WinState.app.input.escape, True); 
    win32_reset_input_state(&WinState.app.input.mouse1, True);
    win32_reset_input_state(&WinState.app.input.mouse2, True);
    win32_reset_input_state(&WinState.app.input.mouse3, True);
    win32_reset_input_state(&WinState.app.input.leftArrow, False);
    win32_reset_input_state(&WinState.app.input.upArrow, False);
    win32_reset_input_state(&WinState.app.input.rightArrow, False);
    win32_reset_input_state(&WinState.app.input.downArrow, False);
    
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }


    HDC dc = GetDC(hwnd);




    // Prep audio
    win32_wasapi_on_frame_start(&WinState);

    // update
    if(firstLoop){
      _log("First update start, dt:%f \n", dt);
    }


    // check cursor lock
    if(WinState.app.cursorLocked){
      if(GetForegroundWindow()==hwnd){
	SetCursorPos(WinState.app.halfScreenWidth, WinState.app.halfScreenHeight);
      }
      
    }
    app_update(dt, &WinState.app);
    if(firstLoop){
      _log("First  update finished, dt:%f \n", dt);
    }
    app_video_update(dt, &WinState.app);
    if(firstLoop){
      _log("First video update finished, dt:%f \n", dt);
    }
    audio_on_update(&WinState.app.audio, dt);

    win32_wasapi_copy_buffer(&WinState);
    if(firstLoop){
      _log("First audio update finished, dt:%f \n", dt);
    }

    // copy buffers
    //copy_buffer_to_window();


    if(currWarmupFrame<warmupFrames){
      _log("Swapping buffer");
    }
    
    SwapBuffers(dc);
    if(currWarmupFrame<warmupFrames){
    _log(" Finished swapping buffer");
    }




        

    /* WinState.audioBufferCurrentWritePosFrames = */
    /*   (WinState.audioBufferCurrentWritePosFrames + numFramesToWrite) */
    /*   % WasapiNumTotalBufferFrames; */

    

  ReleaseDC(hwnd, dc);

  if(WinState.app.requestedClose){
    WinState.running = False;
  }

  if(firstLoop){
    _log("First loop finished, dt:%f \n", dt);
    firstLoop = False;
  }
  }
  

  return 0;
}

internal i32 recursively_read_files_in_directory(const char* path, OSRecursiveDirectoryIterationCallback _callback){

  u32 numAssets = 0;
  

  WIN32_FIND_DATAA findData;
  HANDLE hFind;

  char currentFullPath[WIN32_MAX_PATH_SIZE];

  char relativePath[WIN32_MAX_PATH_SIZE];



  strcpy(relativePath, path);
  u32 len = strlen(relativePath);
  u32 writePos = len;
  if(relativePath[len-1]== '*'){
    writePos = len-1;

  }

      
  strcpy(relativePath+writePos, "/*");

  /* char assetsDirWithStar[MAX_PATH_SIZE]; */
  /* strcpy(assetsDirWithStar, AssetsDir); */
  /* strcpy(assetsDirWithStar + strlen(assetsDirWithStar), "/\*"); */

  
  //_log("Search path: %s\n", relativePath);
  hFind = FindFirstFileA(relativePath, &findData);
  if(hFind == INVALID_HANDLE_VALUE){
    _log("FindFirstFile failed, invalid handle. Dir: %s\n", path);
    return 0;
  }
  //  _log("%s\n", findData.cFileName);
  
  while(hFind!=INVALID_HANDLE_VALUE){
    if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){


      b32 isDot = strcmp(findData.cFileName, ".")==0;
      b32 isDotDot = strcmp(findData.cFileName, "..")==0;
      b32 isRoot = strcmp(findData.cFileName, path)==0;
      //      _log("Dot, DotDot, Root: %d, %d, %d\n", isDot, isDotDot, isRoot);
      if(!isDot && !isDotDot && !isRoot){
	/* _log("Current Relative: %s\n",relativePath); */
	/* _log("cFileName: %s\n",findData.cFileName); */

	char nextPath[WIN32_MAX_PATH_SIZE];
	strcpy(nextPath,relativePath);

	u32 writePos = strlen(relativePath)-1;
	strcpy(nextPath + writePos, findData.cFileName);

	//	_log("Next Path: %s\n", nextPath);
	numAssets+=recursively_read_files_in_directory(nextPath, _callback);
  

      }
      

      
      


      //      GetFullPathName(findData.cFileName, MAX_PATH_SIZE,currentFullPath, NULL);

      /* strcpy(nextPath + strlen(nextPath),nextPattern); */

     
      
     
    }
    else{


      char relativePath[WIN32_MAX_PATH_SIZE];
      strcpy(relativePath, path);
      strcpy(relativePath + strlen(relativePath), "/");
      strcpy(relativePath + strlen(relativePath), findData.cFileName);

      char absolutePath[WIN32_MAX_PATH_SIZE];
      GetFullPathNameA(relativePath, WIN32_MAX_PATH_SIZE,absolutePath, NULL);

      if(_callback!=NULL){
	      _callback(numAssets, relativePath, absolutePath);
      }
      numAssets++;
    }

    if(FindNextFileA(hFind, &findData)){

    } else{

      return numAssets;
    }
    
  }


  


  return numAssets;


}


/////////////////////////////////
// ~sameer: memory implementation

internal mi memory_get_page_size(){
  SYSTEM_INFO sysinfo = {0};
  GetSystemInfo(&sysinfo);

  return sysinfo.dwPageSize;
}
internal void *memory_reserve(mi size){
  void *mem =VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
  Assert(mem);
  return mem;
}
internal void *memory_reserve_at(mi size, void *location){
  void *mem =VirtualAlloc(location, size, MEM_RESERVE, PAGE_NOACCESS);
  Assert(mem);
  return mem;
}
internal b32 memory_commit(void* mem, mi size){
  void *com = VirtualAlloc(mem, size, MEM_COMMIT, PAGE_READWRITE) ;
  Assert(com == mem);
  return com !=NULL;
}

internal b32 memory_decommit(void* mem, mi size){
  return VirtualFree(mem, size, MEM_DECOMMIT);
}
internal b32 memory_free(void* mem, mi size){
return VirtualFree(mem, size, MEM_RELEASE);
}






///////////////////////////////
// ~sameer: file implementation


internal File file_open(string8 path, AccessFlags flags){
  File res = {0};

  DWORD accessFlags = 0;
  DWORD shareMode = 0;
  DWORD creationDisp = OPEN_EXISTING;
  SECURITY_ATTRIBUTES sec = {sizeof(sec), 0, 0};
  if(flags & AccessFlag_Read) accessFlags |= GENERIC_READ;
  if(flags & AccessFlag_Write) {accessFlags |= GENERIC_WRITE; creationDisp = CREATE_ALWAYS; };
  if(flags & AccessFlag_Append) {accessFlags |= FILE_APPEND_DATA; creationDisp = OPEN_ALWAYS; };
  if(flags & AccessFlag_ShareRead) {shareMode |= FILE_SHARE_READ; };
  if(flags & AccessFlag_ShareWrite) {shareMode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE; };
  if(flags & AccessFlag_Inherited){
    sec.bInheritHandle = 1;
  }
  
  HANDLE hnd = CreateFileA((CHAR *)path.ptr, accessFlags, shareMode, &sec, creationDisp, FILE_ATTRIBUTE_NORMAL, NULL);

  if(hnd != INVALID_HANDLE_VALUE){
    res.v[0] = (u64)(hnd);
  }
  else{
    DWORD er=GetLastError();
    (void)er;
    OutputDebugStringA("Invalid file handle, something went wrong here\n");
  }
  return res;
}

internal void file_close(File file){
  if(file_match(file,file_get_empty())) return;

  HANDLE hwnd = (HANDLE)file.v[0];
  BOOL res = CloseHandle(hwnd);
  (void)res;
}
internal mi file_read(File file, Range_mi range, void *outData){
  if(file_match(file,file_get_empty())) 
    {
      outData = NULL;
      return 0;
    };

  mi readAmount = range_mi_length(range);
  Assert(readAmount <= 0xFFFFFFFF);
  HANDLE hnd = (HANDLE)file.v[0];

  DWORD actualReadAmount = 0;
  if (ReadFile(hnd, outData, readAmount, &actualReadAmount, NULL)) {

  }
  else{
    DWORD er=GetLastError();
    (void)er;
    OutputDebugStringA("Invalid file handle, something went wrong here\n");
  }
  return (mi)actualReadAmount;
}

internal b32 file_write(File file, string8 data){
  if(file_match(file,file_get_empty())) return False;
  HANDLE hwnd = (HANDLE)file.v[0];

  DWORD written;
  b32 write = WriteFile(hwnd,data.ptr, data.length, &written, NULL);

  return write && (written == data.length);
}
internal FileProperties file_properties_from_file(File file){

  FileProperties props = {0};
  if(file_match(file,file_get_empty())) return props;


  HANDLE hwnd = (HANDLE)file.v[0];

  BY_HANDLE_FILE_INFORMATION info;
  BOOL good = GetFileInformationByHandle(
					 hwnd,
					 &info
					 );
  if(good){
    //@TODO store more file info received in info struct
    DWORD lowBits = info.nFileSizeLow;
    DWORD highBits = info.nFileSizeHigh;
    u64 size = (u64)lowBits | ( ((u64)highBits) << 32 );

    props.size = size;
  }

  return props;
}


///////////////////////////////
// ~sameer: time implementation
internal u64 time_get_ms_since_boot(){
  ULONGLONG time = GetTickCount64();

  return (u64)time;
}


/////////////////////////////////
// ~sameer: window implementation
void window_get_size(Window win, mi *width, mi *height){
  RECT r= win32_get_window_rect((HWND)win.v[0], width, height);
}

/////////////////////////////////
// ~sameer: cursor implementation
ivec2 cursor_get_pos(){
  POINT point;
  GetCursorPos(&point);
  ivec2 v = {point.x,point.y};
  return v;
}
void cursor_hide(){
  if(WinState.app.cursorHidden) return;
  ShowCursor(False);
  WinState.app.cursorHidden = True;
}
void cursor_show(){
  if(!WinState.app.cursorHidden) return;
  ShowCursor(True);
  WinState.app.cursorHidden = False;
}
void cursor_lock(){
  WinState.app.cursorLocked = True;
}
void cursor_unlock(){
  WinState.app.cursorLocked = False;
}


/* internal void *file_read_whole(string8 path, mi* _contentSize) { */
/*   HANDLE hnd = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL); */
/*   void *content = NULL; */
/*   *_contentSize = 0; */
/*   if (hnd == INVALID_HANDLE_VALUE) { */
/*     return NULL; */
/*   } */
/*   LARGE_INTEGER fileSize; */
/*   if (GetFileSizeEx(hnd, &fileSize)) { */

/*     content = VirtualAlloc(NULL, fileSize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); */
/*     // ReadFile can read a maximum of sizeof(DWORD) bytes */
/*     // so fileSize.QuadPart should be less than that */
/*     Assert(fileSize.QuadPart <= 0xFFFFFFFF); */
/*     DWORD bytesRead=0; */
/*     if (ReadFile(hnd, content, fileSize.QuadPart, &bytesRead, NULL) && (bytesRead == fileSize.QuadPart)) { */

/*     } else { */
/*       memory_free(content, bytesRead); */
/*       content = NULL; */
/*     } */

/*   } else { */
/*   } */

/*   CloseHandle(hnd); */

/*   *_contentSize = fileSize.QuadPart; */
/*   return content; */
/* } */





