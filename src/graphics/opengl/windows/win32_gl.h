#ifndef WIN32_GL_INCLUDE
#define WIN32_GL_INCLUDE


#include <GL/gl.h>
#include <wingdi.h>



global HGLRC win32_gl_context = 0;




// @NOTE wgl reference to get wgl function signatures and other things from: https://registry.khronos.org/OpenGL/api/GL/wglext.h
// @WARNING keep this up to date if something ever changes 


////////////////////////////////////////////////////////////
// ~sameer: wgl functions that we will need to load

typedef BOOL WINAPI func_type_wglChoosePixelFormatARB(HDC hdc, const int *piAttribIList,const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC WINAPI func_type_wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
typedef BOOL WINAPI func_type_wglSwapIntervalEXT(int interval);
func_type_wglChoosePixelFormatARB *wglChoosePixelFormatARB;

func_type_wglCreateContextAttribsARB *wglCreateContextAttribsARB;

func_type_wglSwapIntervalEXT *wglSwapIntervalEXT;


///////////////////////
// ~sameer: wgl defines


#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB       0x2093
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define ERROR_INVALID_VERSION_ARB         0x2095
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#endif // WIN32_GL_INCLUDE
