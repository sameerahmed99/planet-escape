

internal void app_init(AppState *app, Window window){
  _log("initializing gfx");
  gfx_init(window);
  _log("initialized gfx");
  game_init(app);

  _log("initialized game");
}

internal DesiredAppSettings app_get_desired_settings(){
  return game_get_desired_app_settings();
}

internal void app_update(r32 dt, AppState *appState){

  appState->input.mousePosRaw = cursor_get_pos();
  // centered
  appState->input.mousePos = ivec3_create(appState->input.mousePosRaw.x-appState->halfScreenWidth,-appState->input.mousePosRaw.y + appState->halfScreenHeight, 0);

  game_update(dt, appState);

}
internal void app_video_update(r32 dt, AppState *appState){
  game_video_update(dt, appState);
}


internal void game_render_entity(Entity *entity, GameState *state){
  gfx_push_textured_quad(&state->gfxCmdList,entity->tilingOff,color_white,color_white, entity->spriteRawTexture, entity->spriteSize.x,entity->spriteSize.y, entity->pos, entity->zRot);
  
}

