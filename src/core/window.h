#ifndef WINDOW_CORE_INCLUDE
#define WINDOW_CORE_INCLUDE


typedef struct Window Window;

struct Window{
  u64 v[1];
};



/////////////////////////////////////////
// ~sameer: to be implemented by platform
void window_get_size(Window win, mi *width, mi *height);

// cursor

// to be implemented by platform
ivec2 cursor_get_pos();
void cursor_hide();
void cursor_show();
void cursor_lock();
void cursor_unlock();


#endif // WINDOW_CORE_INCLUDE
