#ifndef WIN32_AUDIO_INCLUDE
#define WIN32_AUDIO_INCLUDE


internal void win32_init_wasapi(DesiredAppSettings desired, Win32State *state);

internal void win32_wasapi_copy_buffer(Win32State *state);

internal void win32_wasapi_on_frame_start(Win32State *state);

#endif // WIN32_AUDIO_INCLUDE
