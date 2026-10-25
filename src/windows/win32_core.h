#ifndef WIN32_CORE_INCLUDE
#define WIN32_CORE_INCLUDE








//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fileapi.h>
#include <stdio.h>
#include <winuser.h>
#include <math.h>


#include <synchapi.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>

// for the GET_X_LPARAM and GET_Y_LPARAM macros
#include <windowsx.h>
#include <wingdi.h>
// mingw64 headers need this macro defined
// if you want to use xaudio2 helper macros for calling functions from C instead of C++
// if you go digging, look at the mingw64 headers instead of win32
#define COBJMACROS
//#include <xaudio2.h>
#include <audiosessiontypes.h>



////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: core include, always at the top so we can use some basic functions and macros from the start
#include "../core/core.h"


////////////////////////////////
// ~sameer: third party includes


#include "../core/compiler_warnings_suppress_start.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "../thirdparty/stb_rect_pack.h"
#include "../thirdparty/stb_image.h"
#include "../thirdparty/stb_truetype.h"
#include "../thirdparty/stb_vorbis.h"

#include "../core/compiler_warnings_suppress_end.h"





// @DANGEROUS, windows defines this for some reason, took me a long time to figure it out after i started getting errors in weird places using these as variable names
#undef far
#undef near

////////////////////////
// ~sameer: active game include
#include "../planet_escape/planet_escape_inc.h"





#define WIN32_MAX_PATH_SIZE 512

// REFERENCE_TIME is 100 nano-second units
// so 1 REFERENCE_TIME is 100 nano-seconds
// 1 milliseconds has 1000,000 nano seconds
// so 1 milliseconds has 10,000 REFERENCE_TIME units

#define Win32MsToReftimeUnits(Value) (Value * 10000)
#define Win32SecToReftimeUnits(Value) (Value * 10000000) 




typedef struct Win32ScreenBuffer{
  BITMAPINFO bitmapInfo;
  i32 width,  height;
  u8* data;
  mi stride;
} Win32ScreenBuffer;



typedef struct Win32State{
  AppState app;
  b32 running;
  LARGE_INTEGER performanceQueryFrequency;
  Win32ScreenBuffer screenBuffer;



  IAudioClient *wasapiAudioClient;
  IAudioRenderClient *wasapiRenderClient;


  // unlike keyboard inputs, you can't get last state of mouse clicks, so store manually
  b32 mouse1Pressed,mouse2Pressed,mouse3Pressed;
} Win32State;





/////////////////////////
// ~sameer: win32 globals

global Win32State WinState;





#endif // WIN32_CORE_INCLUDE






