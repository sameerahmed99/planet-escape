#ifndef GUI_CORE_INCLUDE
#define GUI_CORE_INCLUDE



typedef void GuiIDGenericFuncType(void);

typedef struct GuiID GuiID;

struct GuiID
{
  // use either one of these or both of these
  GuiIDGenericFuncType *funcPtrAsID;
  i32 intAsID;
};


typedef struct GuiStyle GuiStyle;
struct GuiStyle{


  TextAlignment textPivotAlignment;
  Font *font;
  i32 fontSize;

  Color textCol;
  Color bgCol;
  Texture *bgTex;

  Color hot_textCol;
  Color hot_bgCol;
  Texture *hot_bgTex;

  Color active_textCol;
  Color active_bgCol;
  Texture *active_bgTex;


};

typedef struct GuiState GuiState;

struct GuiState{

  GfxCmdList *gfxCmdList;
  GuiID hot;
  GuiID active;
  GuiID lastHot;
  GuiID lastActive;

  b32 clickWasReleasedThisFrame;
  b32 holdingClick;

  //@NOTE, manually set z pos of cursor to dictate which overlapping widget should be interactable.
  ivec3 cursorPos;


};


internal void gui_init(GuiState *state);

internal void gui_null_id_func();

internal void gui_on_frame_start(GuiState *state);
internal void gui_set_not_hot(GuiState *state, GuiID id);
internal void gui_set_hot_if_not_active(GuiState *state,GuiID id);
internal void gui_set_active(GuiState *state,GuiID id);


internal b32 gui_is_hot(GuiState *state, GuiID id);
internal b32 gui_has_hot(GuiState *state);
internal b32 gui_is_active(GuiState *state, GuiID id);
internal b32 gui_has_active(GuiState *state);
internal b32 gui_just_became_hot(GuiState *state, GuiID id);

internal b32 gui_hot_changed(GuiState *state);
internal b32 gui_active_changed(GuiState *state);







internal b32 gui_btn(GuiState *state, GuiID id, string8 text, vec3 pos, vec3 halfExtent, GuiStyle style);
internal b32 gui_id_match(GuiID a, GuiID b);

internal b32 gui_mouse_is_in_bounds(GuiState *state, vec3 boundsCenter, vec3 boundsHalfExtent);

internal GuiID gui_id_from_func_ptr(GuiIDGenericFuncType *fun);


#endif // GUI_CORE_INCLUDE
