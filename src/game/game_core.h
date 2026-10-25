#ifndef GAME_CORE_INCLUDE
#define GAME_CORE_INCLUDE


//////////////////////
// ~sameer: core types


typedef struct InputState{
  b32 wasDownedThisFrame;
  b32 isPressed;
  b32 wasReleasedThisFrame;
} InputState;


typedef struct Input {
  InputState w;
  InputState a;
  InputState s;
  InputState d;
  InputState leftArrow, upArrow, rightArrow, downArrow;

  InputState q;
  InputState e;
  InputState lshift;
  InputState lalt;
  InputState rshift;
  InputState ralt;

  InputState space;
  InputState escape;

  InputState mouse1, mouse2, mouse3;
  ivec3 mousePos;
  ivec2 mousePosRaw;
  r32 mouseDeltaX, mouseDeltaY;
  
} Input;



typedef struct AppState{

  // these are ints deliberately, don't want unexpected bugs when using negative values during calculations
  i32 screenWidth;
  i32 screenHeight;
  i32 halfScreenWidth, halfScreenHeight;
  r32 renderScale;
  i32 audioChannelsCount;
  i32 audioSampleRate;
  i32 audioBitDepth;
  r32 audioBufferSizeInSeconds;
  i32 audioBufferBytesPerFrame;
  i32 audioBufferSizeInFrames;
  i32 audioBufferSizeInBytes;  
  Input input;

  Arena *arena;

  // stop using dedicated audioArena, seems unecessary. Use common arena
  Arena *audioArena;
  AudioState audio;

  b32 requestedClose;
  b32 cursorLocked;
  b32 cursorHidden;
} AppState;




typedef struct DesiredAppSettings{
  i32 screenWidth;
  i32 screenHeight;

  // ignore requested screenWidth and screenHeight if true
  b32 fullScreen;


  r32 renderScale;
  i32 audioChannelsCount;
  i32 audioSampleRate;
  i32 audioBitDepth;
  r32 audioBufferSizeInSeconds;


} DesiredAppSettings;



typedef struct GameState{
  b32 appInitialized;
  Arena *entitiesArena;

  // physFrameArena should be separate from frame arena since we are doing a unity style fixed update where the each actual game frame can call 0 to n fixed updates
  Arena *physFrameArena;

  Arena *frameArena;
  Arena *scratchArena;
  Arena *graphicsCmdArena;
  Arena *assetsArena;
  GfxCmdList gfxCmdList;
  r32 physAccumulator;
  r32 physTimeStep;
  b32 paused;

  mi numCollisionsCurrPhysFrame;
  CollisionInfo *currPhysFrameFirstCollision;
  CollisionInfo *currPhysFrameLastCollision;
} GameState;








internal DesiredAppSettings app_get_desired_settings();
internal void app_update(r32 dt, AppState *appState);
internal void app_video_update(r32 dt, AppState *appState);

internal void app_init(AppState *app, Window window);

//////////////////////////////////////////
// ~sameer: to be implemented by user apps
internal void game_init(AppState *app);
internal void game_update(r32 dt, AppState *appState);
internal void game_video_update(r32 dt, AppState *appState);
internal DesiredAppSettings game_get_desired_app_settings();
internal void game_render_entity(Entity *entity, GameState *state);
internal void game_on_physics_update();
#endif // GAME_CORE_INCLUDE
