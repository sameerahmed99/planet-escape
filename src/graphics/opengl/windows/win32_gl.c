// @NOTE wgl reference: https://registry.khronos.org/OpenGL/api/GL/wglext.h

// core implemntations



internal void gl_os_init(Window window){
  HWND hwnd = (HWND)window.v[0];
  PIXELFORMATDESCRIPTOR pfd =
    {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
      PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
      32,                   // Colordepth of the framebuffer.
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      24,                   // Number of bits for the depthbuffer
      8,                    // Number of bits for the stencilbuffer
      0,                    // Number of Aux buffers in the framebuffer.
      PFD_MAIN_PLANE,
      0,
      0, 0, 0
    };

  HDC dc = GetDC(hwnd);

  int bestFormatNumber;
  bestFormatNumber = ChoosePixelFormat(dc, &pfd);

  BOOL describe = DescribePixelFormat(dc, bestFormatNumber, sizeof(pfd), &pfd);
  SetPixelFormat(dc,bestFormatNumber, &pfd);

  HGLRC tempContext = wglCreateContext(dc);
  wglMakeCurrent (dc, tempContext);

  wglChoosePixelFormatARB = (func_type_wglChoosePixelFormatARB*)gl_get_func_address("wglChoosePixelFormatARB");
  wglCreateContextAttribsARB = (func_type_wglCreateContextAttribsARB*)gl_get_func_address("wglCreateContextAttribsARB");
  wglSwapIntervalEXT = (func_type_wglSwapIntervalEXT*)gl_get_func_address("wglSwapIntervalEXT");


  const int attribList[] =
    {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 32,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB, 8,
      0, // End
    };


  int numFormats=0;
  BOOL gotFormat=wglChoosePixelFormatARB(dc, attribList, 0,1,&bestFormatNumber,&numFormats);

  HGLRC realContext = 0;
  if(gotFormat){

    //https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_create_context.txt

    //@NOTE, using opengl 3.3 for maximum compatibility. Look into the versions if this becomes a problem. These are interpreted as key value pairs.
    int realAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB,3,
      WGL_CONTEXT_MINOR_VERSION_ARB,3,
      WGL_CONTEXT_LAYER_PLANE_ARB,0,
      WGL_CONTEXT_FLAGS_ARB,0,
      WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0 // @NOTE, this 0 is required at the end to terminate the list of attribus.
    };
    realContext = wglCreateContextAttribsARB(dc, tempContext, realAttribs);
    if(realContext == 0){
      DWORD err=GetLastError();
      (void)err;
      MessageBoxA(0,"Context is null, error", "OPENGL CONTEXT CREATION FAILED",0);
    }
  }
  else{

  }
  

  // cleanup
  wglMakeCurrent(dc, 0);
  wglDeleteContext(tempContext);
  wglMakeCurrent(dc, realContext);
  win32_gl_context = realContext;

  
  if(realContext){
    //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
    OutputDebugStringA("OpenGL context created");
  }
  else{

  }

  
}


internal void *gl_get_func_address(char *name){
  void *p = (void *)wglGetProcAddress(name);
  if(p == 0 ||
    (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
    (p == (void*)-1) )
  {
    /* HMODULE module = LoadLibraryA("opengl32.dll"); */
    /* p = (void *)GetProcAddress(module, name); */
    p=0;
  }

  return p;
}

internal void gl_swap_interval(i32 val){
  wglSwapIntervalEXT(val);
}
