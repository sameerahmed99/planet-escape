internal GuiID gui_nullID;


internal void gui_init(GuiState *state){
  gui_nullID = gui_id_from_func_ptr(&gui_null_id_func);
  state->active = gui_nullID;
  state->hot = gui_nullID;
  state->lastHot = gui_nullID;
  state->lastActive = gui_nullID;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: instead of using a NULL pointer, which the user may pass as an id for some reason, use this specific function to get a guaranteed unique null id
// should never get called.
internal void gui_null_id_func(){
  _log("Who awakens the sleeping one?");
  Assert(False);
}


internal void gui_on_frame_start(GuiState *state){
  state->lastHot = state->hot;
  state->lastActive = state->active;
}

internal b32 gui_has_active(GuiState *state){
  return (!gui_id_match(state->active, gui_nullID));
}
internal b32 gui_has_hot(GuiState *state){
  return (!gui_id_match(state->hot, gui_nullID));
}
internal void gui_set_hot_if_not_active(GuiState *state,GuiID id){

  if(gui_has_active(state)) return;
  if(gui_is_hot(state,id)) return;
  state->hot = id;

}
internal void gui_set_not_hot(GuiState *state,GuiID id){
  state->hot = gui_nullID;
}
internal void gui_set_active(GuiState *state,GuiID id){
  if(gui_has_hot(state)){
    gui_set_not_hot(state, state->hot);
  }
  state->active = id;
}

internal void gui_set_not_active(GuiState *state,GuiID id){
  state->active = gui_nullID;
}
internal b32 gui_id_match(GuiID a, GuiID b){
  if(a.funcPtrAsID != b.funcPtrAsID) return False;
  if(a.intAsID != b.intAsID) return False;
  return True;
}
internal b32 gui_is_hot(GuiState *state, GuiID id){
  return gui_id_match(state->hot, id);
}


internal b32 gui_just_became_hot(GuiState *state, GuiID id){
  return gui_is_hot(state, id) && !gui_id_match(state->lastHot, state->hot);
}
internal b32 gui_hot_changed(GuiState *state){
  return !gui_id_match(state->hot, state->lastHot) && gui_has_hot(state);
}
internal b32 gui_active_changed(GuiState *state){
  return !gui_id_match(state->active, state->lastActive) && gui_has_active(state);
}
internal b32 gui_is_active(GuiState *state, GuiID id){
  return gui_id_match(state->active, id);
}

internal b32 gui_btn(GuiState *state, GuiID id, string8 text, vec3 pos, vec3 halfExtent, GuiStyle style){
  b32 trigerred = False;
  b32 active = False, hot = False;
  b32 bounds=gui_mouse_is_in_bounds(state, pos, halfExtent);
  if(gui_is_active(state,id)){

    if(state->clickWasReleasedThisFrame){

      gui_set_not_active(state, id);
      if(bounds){
	trigerred = True;
	gui_set_hot_if_not_active(state,id);
      }
    }
  }
  else{
    if(bounds){
      if(!gui_has_active(state)){
	if(state->holdingClick){
	  gui_set_active(state,id);
	}
	else{
	  gui_set_hot_if_not_active(state,id);
	}
      }
      
    }
    else if(gui_is_hot(state, id)){
      gui_set_not_hot(state,id);
    }
  }



  


  active = gui_is_active(state, id);
  hot = gui_is_hot(state, id);



  
  Color clampCol = color_white;
  Color bgCol = hot ? style.hot_bgCol : (active ? style.active_bgCol : style.bgCol);
  Color textCol = hot ? style.hot_textCol : (active ? style.active_textCol : style.textCol);
  Texture *bgTex = hot ? style.hot_bgTex : (active ? style.active_bgTex : style.bgTex);

  gfx_push_textured_quad(state->gfxCmdList, tiling_and_offset_identity,clampCol,bgCol, bgTex, halfExtent.x*2,halfExtent.y*2, pos,0);
  gfx_push_text(state->gfxCmdList, style.font, text,style.fontSize,pos, style.textPivotAlignment,textCol);


  return trigerred;
}

internal b32 gui_mouse_is_in_bounds(GuiState *state, vec3 boundsCenter, vec3 boundsHalfExtent){
  if(!r32_match_approx(boundsCenter.z, state->cursorPos.z)) return False;

  b32 x = state->cursorPos.x >= boundsCenter.x - boundsHalfExtent.x;
  x &= state->cursorPos.x <= boundsCenter.x + boundsHalfExtent.x;

  b32 y = state->cursorPos.y >= boundsCenter.y - boundsHalfExtent.y;
  y &= state->cursorPos.y <= boundsCenter.y + boundsHalfExtent.y;

  return x && y;
}
internal GuiID gui_id_from_func_ptr(GuiIDGenericFuncType *fun){
  GuiID id = {.funcPtrAsID = fun, .intAsID = 0};

  return id;
}
