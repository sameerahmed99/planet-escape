#include <stdlib.h>

#define gJmpNumParticleSystems 16
internal const r32 gJmpMovingPlatformSpeedMin = 5;
internal const r32 gJmpMovingPlatformSpeedMax = 15;
internal const r32 gJmpMovingPlatformMinHeight = 750;
internal const r32 gJmpMovingPlatformChance = 0.35f;
internal const r32 gJmpPlatformMaxXOffset=20;


//@TODO, create particles system in the game core folder 
typedef struct JmpParticle{
  b32 alive;
  r32 duration;
  r32 age;
  r32 startScale;
  r32 endScale;
  vec3 vel;
  vec3 localPos;
  vec3 angles;
  r32 startOpacity;
  r32 endOpacity;
  mat4x4 mat;
}JmpParticle;


internal i32 jmp_FontSizeDefault = 30;
internal i32 jmp_FontSizeScore = 50;

typedef struct JumpGame{
  GameState state;
  AppState *app;
  JmpParticle particles[gJmpNumParticleSystems];

  Texture texTest;
  Texture texGrid;
  Texture texHero;
  Texture texWhite;
  Texture texPlatform;
  Texture texMovingPlatformFrame;
  Texture texPlatformDead;
  Texture texThrusterFuel;
  Texture texCircleCol;
  Texture texRectCol;
  Texture texBg;
  Texture texThrust;
  Font fontDefault;
  Texture fontDefaultTex;
  mat4x4 projMat, viewMat;
  mat4x4 projMatScreenCenter, viewMatScreenCenter;


  Entity *stickman;

  u32 gridSize;
  r32 worldWidthInScreen, worldHeightInScreen;

  
  r32 platformSpawnOffset;

  r32 viewOffsetFromPlayer;
  r32 playerZOffset, viewZOffset;

  r32 startingPlatformOffset;

  r32 lastPlatformPos;

  r32 maxHeightReached;
  r32 prevBest;
  b32 failed;

  AudioBuffer TestSong;
  QueuedAudioBuffer *TestQueued;
  AudioBuffer KickDrum;
  AudioBuffer JumpSfx;
  AudioBuffer JumpDeadSfx;
  AudioBuffer SpaceCraftThrustSfx;
  AudioBuffer BtnHotSfx;
  AudioBuffer BtnActiveSfx;

  r32 secondsSinceFpsUpdate;
  r32 lastFrameTime;

  i32 tempSwapInterval;

  GuiStyle guiStyleDefault;
  GuiState gui;



  r32 maxHSpeed;
  r32 accel;

  i32 chargesPerJump;

  i32 charges;
  i32 maxCharges;

  b32 leftInput, rightInput;



} JumpGame;



internal JumpGame *Game;


internal Entity *spc_create_platform(vec3 pos){

  Entity *TestFloor = entity_create(Game->state.entitiesArena, ENTITY_KIND_PLATFORM, pos, mat4x4_identity);
  TestFloor->debugString = str8_lit("Floor2");


  TestFloor->killOnRestart = True;
  TestFloor->hasSprite = True;
  TestFloor->spriteRawTexture =&Game->texPlatform;
  TilingAndOffset to ={.tiling = {1,1}, .offset={0,0}};
  TestFloor->tilingOff = to;


  TestFloor->physics = True;
  TestFloor->isPhysBody = True;
  TestFloor->staticPhysics = False;
  TestFloor->obeyGravity = False;
  TestFloor->velocity = vec3_zero;
  PhysFlags pf = {.v= PHYS_FLAG_ALL_DEFAULT};
  vec3 colSize = {5,5,10};
  TestFloor->colliderChain = entity_create_cuboid_collider(Game->state.entitiesArena, TestFloor, colSize,pf);
  TestFloor->hasCollider = True;

  TestFloor->spriteSize.x = colSize.x*2;
  TestFloor->spriteSize.y = colSize.y*2;

  
  return TestFloor;
}
internal void spc_create_first_platform(){
  vec3 pos = {0, -Game->worldHeightInScreen/2+Game->startingPlatformOffset, 0.0f};
  spc_create_platform(pos);
  Game->lastPlatformPos = pos.y;
}

  internal void spc_load_prev_best(){
    string8 existing=file_read_from_path_into_buffer(Game->state.frameArena, str8_lit("save.txt"));
    r32 prevBest = string8_to_r32(existing);
    Game->prevBest = prevBest;
  }

internal void spc_restart(){
  spc_load_prev_best();
  Game->stickman->experiencedGravity = vec3_create(0,-75,0);
  Game->failed = False;
  entity_teleport(Game->stickman, vec3_zero);
  Game->stickman->velocity = vec3_zero;
  Game->viewMat = mat4x4_translation(0,0,-1.0f);
  Game->maxHeightReached = 0;
  Game->charges = 5;
  Game->leftInput = False;
  Game->rightInput = False;


  i32 numEnt=CountStructs(Game->state.entitiesArena, Entity);

  for(int i=0;i<numEnt;i++){
    Entity *en = GetStruct(Game->state.entitiesArena, Entity, i);
    if(en->killOnRestart) {
      en->alive = False;
    }
  }
  spc_create_first_platform();


  for(int i=0;i<gJmpNumParticleSystems;i++){
    JmpParticle *ps=&Game->particles[i];
    ps->alive = False;

  }
 
  
}
internal void spc_close(){
  Game->app->requestedClose = True;
}
internal void spc_menu(){


  gfx_push_textured_quad(&Game->state.gfxCmdList, tiling_and_offset_identity,color_white,color_create(0,0,0,0.7f), &Game->texWhite, Game->app->screenWidth,Game->app->screenHeight, vec3_zero,0);

  r32 currentY = 0;

  GuiStyle style = Game->guiStyleDefault;

  style.hot_textCol = color_create(0.3f,0.8f,.5f,1.0f);
  style.active_textCol = color_create(0.15f,0.6f,.35f,1.0f);

  style.bgCol=color_create(0.3f,0.8f,.5f,0.0f);
  style.hot_bgCol = color_create(0.4f,0.8f,.55f,0.0f);
  style.active_bgCol = color_create(0.4f,0.8f,.55f,0.0f);

  r32 btnHalfExtent = 20;
  r32 btnMargin = 5;
  if(Game->failed){
    i32 reachedInt = Game->maxHeightReached;
    string8 reachedString=str8cat(Game->state.frameArena, i32_to_string8(Game->state.frameArena, reachedInt),str8_lit(" meters "));
    string8 prevBestString=str8cat(Game->state.frameArena, str8_lit("Previous best: "),i32_to_string8(Game->state.frameArena, (i32)Game->prevBest));
    prevBestString = str8cat(Game->state.frameArena, prevBestString, str8_lit(" meters"));

    Color col;
    if(Game->prevBest <= Game->maxHeightReached){
      reachedString=str8cat(Game->state.frameArena, reachedString, str8_lit("(New best!)"));

      col = color_create(0.3f,0.8f,0.3f,1.0f);
    }
    else{

      col = color_create(0.9f,0.9f,0.9f,1.0f);
    }



    
    gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, reachedString,40,vec3_create(0,130,0), TEXT_ALIGN_CENTER_MIDDLE,col);
    gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, prevBestString,30,vec3_create(0,90,0), TEXT_ALIGN_CENTER_MIDDLE,color_create(1,1,1,0.6f));
  }


  if(gui_btn(&Game->gui, gui_id_from_func_ptr(&spc_restart),str8_lit("Restart"),vec3_create(0,currentY,0), vec3_create(75,btnHalfExtent,1),style)){
    spc_restart();
    Game->state.paused = False;

  }
  currentY-=(btnHalfExtent*2+btnMargin);

  if(gui_btn(&Game->gui, gui_id_from_func_ptr(&spc_close),str8_lit("Exit"),vec3_create(0,currentY,0), vec3_create(75,btnHalfExtent,1),style)){
    spc_close();
  }

  if(gui_hot_changed(&Game->gui) && !gui_id_match(Game->gui.hot, Game->gui.lastActive)){
    QueuedAudioBuffer *hotsfx=audio_queue_buffer(&Game->app->audio, &Game->BtnHotSfx, 0.3f);
  }
  if(gui_active_changed(&Game->gui)){
    QueuedAudioBuffer *activeSfx=audio_queue_buffer(&Game->app->audio, &Game->BtnActiveSfx,0.3f);
  }

  currentY-=(btnHalfExtent*2+btnMargin);

}



  internal void spc_update_gui_input(ivec3 cursorPos, b32 holdingClick, b32 mouseUp){
    Game->gui.cursorPos = cursorPos;
    Game->gui.clickWasReleasedThisFrame = mouseUp;
    Game->gui.holdingClick = holdingClick;
  }



internal void game_init(AppState *app){
  cursor_lock();
  cursor_hide();
  mi gameMemSize = MB(5);
  Assert(sizeof(*Game) < gameMemSize);
  Game = memory_reserve(gameMemSize);
  memory_commit(Game, gameMemSize);
  
  _log("initializing space game");

  Game->app = app;

  Game->state.appInitialized = True;
  Game->state.physTimeStep = 0.02f;
  Game->state.paused = False;


  
  Game->state.entitiesArena = arena_create(MB(1),MB(100));

  Game->state.frameArena = arena_create(MB(1),MB(100));
  Game->state.physFrameArena = arena_create(MB(1),MB(100));
  Game->state.scratchArena = arena_create(MB(1),MB(100));
  Game->state.graphicsCmdArena = arena_create(MB(1),MB(100));
  Game->state.assetsArena = arena_create(MB(10),MB(100));
  
  Game->state.gfxCmdList.arena = Game->state.graphicsCmdArena;

  _log("loading assets");

  Game->TestSong=audio_buffer_from_file_path_ogg_vorbis_16bit(Game->state.assetsArena,str8_lit("../assets/audio/TheLastDeliveryManOnEarth.ogg"));
  Game->KickDrum=audio_buffer_from_file_path_ogg_vorbis_16bit(Game->state.assetsArena,str8_lit("../assets/audio/KickDrum.ogg"));
  Game->JumpSfx=audio_buffer_from_file_path_ogg_vorbis_16bit(Game->state.assetsArena,str8_lit("../assets/audio/SpaceCraftJump.ogg"));
  Game->SpaceCraftThrustSfx=audio_buffer_from_file_path_ogg_vorbis_16bit(Game->state.assetsArena,str8_lit("../assets/audio/SpaceCraftThrust.ogg"));
  Game->BtnHotSfx=audio_buffer_from_file_path_ogg_vorbis_16bit(Game->state.assetsArena,str8_lit("../assets/audio/BtnHot.ogg"));
  Game->BtnActiveSfx=audio_buffer_from_file_path_ogg_vorbis_16bit(Game->state.assetsArena,str8_lit("../assets/audio/BtnActive.ogg"));

  Game->maxHSpeed = 25;
  Game->chargesPerJump = 1;
  Game->maxCharges = 10;
  spc_load_prev_best();

  {
    TempArena temp = arena_temp_begin(Game->state.scratchArena);
    


    //@TODO, font creation function
    //string8 fontFile = str8_lit("../assets/fonts/Overpass/static/Overpass-Regular.ttf");
    string8 fontFile = str8_lit("../assets/fonts/public_sans/public_sans_regular.ttf");
    string8 data = file_read_from_path_into_buffer(temp.arena,fontFile);

    Font *font = &Game->fontDefault;
    font->sizePx = 120;
    stbtt_InitFont(&font->stbFont, data.ptr, stbtt_GetFontOffsetForIndex(data.ptr,0));
    r32 scaleForPixelHeight=stbtt_ScaleForPixelHeight(&font->stbFont, font->sizePx);

    stbtt_pack_context ctx;

    i32 startIndex = 32;
    i32 endIndex = 126;
    int numChars = endIndex - startIndex + 1;

    i32 texSize = 1024;

    Game->fontDefaultTex = texture_create_empty(texSize, texSize,GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST);
    font->tex = &Game->fontDefaultTex;
    
    
    string8 packedBitmap;

    //@FIXME, malloc
    packedBitmap.ptr = malloc(sizeof(char)*texSize*texSize);
    stbtt_PackBegin(&ctx, (char*)packedBitmap.ptr, texSize, texSize,texSize, 1, NULL);
    //stbtt_PackSetOversampling(&ctx, 1, 1);
    _log("font packing used malloc\n");
    font->stbCharData = malloc(sizeof(stbtt_packedchar)*numChars);




    //@NOTE there is a function stbtt_PackFontRanges that can take multiple ranges, useful for loading unicode ranges in other languages not included in ascii
    // @NOTE pass pixel font size instead of scale, see stbtt_PackFontRange implementation, follow along and see how it uses the font_size. You'll see that it does the scaling already.
    stbtt_PackFontRange(&ctx, (char*)data.ptr, 0, font->sizePx, startIndex, numChars, font->stbCharData);
    stbtt_PackEnd(&ctx);

    /* bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, 128), c, &w, &h, 0,0); */


    /* string8 buffer; */

    /* buffer.ptr= arena_push(temp.arena, sizeof(u8)*(w*h*4),sizeof(u8), True); */

    /* Texture tex = texture_create_empty(w,h,GFX_TEXTURE_WRAP_MODE_REPEAT, GFX_TEXTURE_FILTERING_NEAREST); */
    
    /* u8 *p = (u8*)bitmap; */


    /* u8 *dstRow = buffer.ptr +  (tex.height - 1)*tex.rowStride; */

    /* for(int y=0;y<h;y++){ */


    /*   for(int x=0;x<w;x++){ */
    /* 	u8 *dst = dstRow + x*tex.channels; */
    /* 	u8 alpha = *p; */
    /* 	dst[0] = alpha; */
    /* 	dst[1] = alpha; */
    /* 	dst[2] = alpha; */
    /* 	dst[3] = alpha; */

	
    /* 	p++; */
    /*   } */
    /*   dstRow-= tex.rowStride; */
    /* } */


    /* texture_init_empty(&tex, buffer); */


    /* test = tex; */

    u8 *p = (u8*)packedBitmap.ptr;

    string8 packedBuffer;
    packedBuffer.ptr = arena_push(temp.arena,(texSize*texSize*sizeof(u8)*4), sizeof(u8)*TEXTURE_NUM_CHANNELS,True);
    
    u8 *dstRow = packedBuffer.ptr +  (font->tex->height - 1)*font->tex->rowStride;




    
    for(int y=0;y<texSize;y++){


      for(int x=0;x<texSize;x++){
	u8 *dst = dstRow + x*font->tex->channels;
	u8 alpha = *p;

	// not premultipled
	dst[0] = 255;
	dst[1] = 255;
	dst[2] = 255;
	dst[3] = alpha;


	////////////
	// ~sameer: premultiplied, alpha has already scaled the white rgb channel
	// if you use this to try and reduce artifacts, make sure you use GL_ONE for the source factor in opengl's blend function

	/* dst[0] = alpha; */
	/* dst[1] = alpha; */
	/* dst[2] = alpha; */
	/* dst[3] = alpha; */

	
	p++;
      }
      dstRow-= font->tex->rowStride;
    }
    texture_init_empty(Game->state.assetsArena, font->tex, packedBuffer, False);


    /* stbtt_FreeBitmap(bitmap,NULL); */
    free(packedBitmap.ptr);
    arena_temp_end(temp);



  }
  



  Game->texTest = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/SpaceBackgroundLast.png"),True, GFX_TEXTURE_WRAP_MODE_REPEAT, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texGrid = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/GridBorders.png"),True, GFX_TEXTURE_WRAP_MODE_REPEAT, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texHero = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/SpaceCraft.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texCircleCol = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/CircleCollider.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texRectCol = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/RectangleCollider.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texPlatform = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/SpacePlatform2D.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texPlatformDead = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/DeadSpacePlatform2D.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texMovingPlatformFrame = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/moving_platform_frame.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texBg = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/PlanetBackground.png"),True, GFX_TEXTURE_WRAP_MODE_REPEAT, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texThrust = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/SpaceCraftThrustParticle.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texThrusterFuel = texture_load_from_file(Game->state.assetsArena,str8_lit("../assets/textures/thruster_fuel.png"),True, GFX_TEXTURE_WRAP_MODE_CLAMP, GFX_TEXTURE_FILTERING_NEAREST, False);
  Game->texWhite = texture_create(Game->state.assetsArena,32,32, color_white,GFX_TEXTURE_WRAP_MODE_REPEAT, GFX_TEXTURE_FILTERING_NEAREST, False);
  
  if(!gfx_renderer_requires_textures_to_be_kept_on_cpu_memory()){
    texture_free_data(Game->texTest);
    texture_free_data(Game->texGrid);
    texture_free_data(Game->texHero);
    texture_free_data(Game->texCircleCol);
    texture_free_data(Game->texRectCol);
    texture_free_data(Game->texPlatform);
    texture_free_data(Game->texPlatformDead);
    texture_free_data(Game->texMovingPlatformFrame);
    texture_free_data(Game->texBg);
    texture_free_data(Game->texThrust);
    texture_free_data(Game->texThrusterFuel);
    
    //texture_free(Game->texWhite);
    _log("renderer doesn't need texture in cpu memory, released.\n");
  }

  {
    r32 aspect = (r32)app->screenWidth / app->screenHeight;

    r32 width = 100;
    r32 height = width / aspect;
    r32 hw = width/2;
    r32 hh = height/2;
    Game->worldWidthInScreen = width;
    Game->worldHeightInScreen=height;
    Game->platformSpawnOffset = 20;
    Game->viewOffsetFromPlayer = 1;
    Game->startingPlatformOffset = Game->viewOffsetFromPlayer;
    Game->viewZOffset = -10.0f;
    Game->playerZOffset = 1.0f;
    Game->projMat = mat4x4_projection_ortho(hw, -hw, hh, -hh, 100, 0.01f);

  }

  {

    r32 hw = app->screenWidth/2;
    r32 hh = app->screenHeight/2;
    Game->projMatScreenCenter = mat4x4_projection_ortho(hw, -hw, hh, -hh, 100, 0.01f);
    Game->viewMatScreenCenter = mat4x4_translation(0,0,-0.1f);
  }



  //Game->viewMat = mat4x4_identity;

  // move it outwards from the screen a bit so that things sitting on 0 on the Z axis are still rendered without the need to move them back a bit
  // i say move outwards but remember this is a viewMatrix applied directly to the transformation, negative z means "camera" is moving outwards from the screen or objects are going inwards

  _log("creating scene");
  Game->viewMat = mat4x4_translation(0,0,Game->viewZOffset);
  {
    spc_create_first_platform();
    /* vec3 pos = {0, -Game->worldHeightInScreen/2+Game->startingPlatformOffset, 0.0f}; */
    /* spc_create_platform(pos); */
    /* Game->lastPlatformPos = pos.y; */
    
    
    /* vec3 pos = {0,-Game->worldHeightInScreen/2+Game->startingPlatformOffset,-0.1f}; */
    /* Entity *TestFloor = entity_create(Game->state.entitiesArena, ENTITY_KIND_STATIC, pos, mat4x4_identity); */
    /* TestFloor->kind = ENTITY_KIND_STATIC; */
    /* TestFloor->debugString = str8_lit("Floor"); */





    /* TestFloor->alive = True; */
    /* TestFloor->generation = 0; */
    /* TestFloor->hasSprite = False; */
    /* TestFloor->spriteRawTexture =Game->texWhite; */
    /* TilingAndOffset to ={.tiling = {1,1}, .offset={0,0}}; */
    /* TestFloor->tilingOff = to; */
    /* TestFloor->spriteSize.x = 25; */
    /* TestFloor->spriteSize.y = 5; */

    /* TestFloor->physics = True; */
    /* TestFloor->isPhysBody = True; */
    /* TestFloor->staticPhysics = True; */
    /* TestFloor->obeyGravity = False; */
    /* PhysFlags pf = {.v= PHYS_FLAG_ALL_DEFAULT}; */
    /* vec3 colSize = {10,.2f,10}; */
    /* TestFloor->colliderChain = entity_create_cuboid_collider(Game->state.entitiesArena, TestFloor, colSize,pf); */
    /* TestFloor->hasCollider = True; */
  }

  {
    Game->charges = 5;
  }
  /* { */
  /*   vec3 pos = {-15,-Game->worldHeightInScreen/2 + Game->platformSpawnOffset,-0.1f}; */
  /*   Entity *TestFloor = entity_create(Game->state.entitiesArena, ENTITY_KIND_STATIC, pos, mat4x4_identity); */
  /*   TestFloor->kind = ENTITY_KIND_STATIC; */
  /*   TestFloor->debugString = str8_lit("Floor2"); */





  /*   TestFloor->alive = True; */
  /*   TestFloor->generation = 0; */
  /*   TestFloor->hasSprite = False; */
  /*   TestFloor->spriteRawTexture =Game->texWhite; */
  /*   TilingAndOffset to ={.tiling = {1,1}, .offset={0,0}}; */
  /*   TestFloor->tilingOff = to; */
  /*   TestFloor->spriteSize.x = 25; */
  /*   TestFloor->spriteSize.y = 5; */

  /*   TestFloor->physics = True; */
  /*   TestFloor->isPhysBody = True; */
  /*   TestFloor->staticPhysics = True; */
  /*   TestFloor->obeyGravity = False; */
  /*   PhysFlags pf = {.v= PHYS_FLAG_ALL_DEFAULT}; */
  /*   vec3 colSize = {5,.2f,10}; */
  /*   TestFloor->colliderChain = entity_create_cuboid_collider(Game->state.entitiesArena, TestFloor, colSize,pf); */
  /*   TestFloor->hasCollider = True; */
  /* } */
  {
    // move it infront of other things
    vec3 pos = {0,0 , 0.0f, Game->playerZOffset};
    Game->stickman = entity_create(Game->state.entitiesArena, ENTITY_KIND_PLAYER, pos, mat4x4_identity);
    Game->stickman->debugString = str8_lit("Game->stickman");
    Game->stickman->kind = ENTITY_KIND_PLAYER;
    Game->stickman->alive = True;
    Game->stickman->generation = 0;
    Game->stickman->hasSprite = True;
    //Game->stickman->spriteRawTexture = &Game->texHero;
    Game->stickman->spriteRawTexture = &Game->texHero;
    //Game->stickman->spriteRawTexture = Game->fontDefault.tex;
    TilingAndOffset to ={.tiling = {1,1}, .offset={0,0}};
    Game->stickman->tilingOff = to;
    Game->stickman->spriteSize.x = 5;
    Game->stickman->spriteSize.y = 5;
    Game->stickman->physics = True;
    Game->stickman->isPhysBody = True;
    Game->stickman->staticPhysics = False;
    Game->stickman->obeyGravity = True;
    Game->stickman->mass = 75;
    Game->stickman->experiencedGravity = vec3_create(0,-75,0);
    PhysFlags pf = {.v= PHYS_FLAG_ALL_DEFAULT};

    vec3 colSize = {2.5f,2.5f,2.5f};
    Game->stickman->colliderChain = entity_create_cuboid_collider(Game->state.entitiesArena, Game->stickman, colSize,pf);
    Game->stickman->hasCollider = True;
    
  }


  phys_init(&Game->state);



  GuiStyle guiStyleDefault = {
    .textPivotAlignment = TEXT_ALIGN_CENTER_MIDDLE,
    .font=&Game->fontDefault,
    .fontSize= 40,
    .textCol=color_white,
    .bgCol = color_create(0.2f,0.2f,0.2f,1.0f),
    .bgTex=&Game->texWhite,

    .hot_textCol=color_white,
    .hot_bgCol=color_create(0.35f,0.35f,0.35f,1.0f),
    .hot_bgTex=&Game->texWhite,

    .active_textCol=color_white,
    .active_bgCol=color_create(0.5f,0.5f,0.5f,1.0f),
    .active_bgTex=&Game->texWhite,
  };
  Game->gui.gfxCmdList=&Game->state.gfxCmdList;

  Game->guiStyleDefault = guiStyleDefault;

  Input input = app->input;
  spc_update_gui_input(input.mousePos, input.mouse1.isPressed, input.mouse1.wasReleasedThisFrame);

  _log("space game initialized");

  gui_init(&Game->gui);
}

DesiredAppSettings game_get_desired_app_settings(){
  DesiredAppSettings set={
    .screenWidth=1600,
    .screenHeight=900,
    .fullScreen = True,
    .renderScale=1,
    .audioChannelsCount=2,
    .audioSampleRate=48000,
    .audioBitDepth=16,
    .audioBufferSizeInSeconds=.05f,
    // use a big buffer size for debugging audio issues
    //.audioBufferSizeInSeconds=2.0f,

  };
  return set;
}



internal r32 spc_generate_platforms(vec3 playerPos, r32 platformOffset, r32 lastPlatformPos, r32 maxY, i32 minMaxX){

  if(platformOffset>=maxY) return lastPlatformPos;

  
  i32 num = (maxY - lastPlatformPos) / platformOffset;

  r32 last=lastPlatformPos; 
  for(int i=0;i<num;i++){

    
    r32 x= Rand11() * minMaxX;


    vec3 pos = {x,lastPlatformPos + platformOffset*(i+1),0};
    Entity *plat=spc_create_platform(pos);

    if(pos.y>=gJmpMovingPlatformMinHeight){
      b32 shouldMove = Rand01() <= gJmpMovingPlatformChance;
      plat->isMovingPlatform = shouldMove;
      if(shouldMove){

	r32 sign = Rand01()>=0.5f ? 1 : -1;
	r32 vel = Lerp(gJmpMovingPlatformSpeedMin,gJmpMovingPlatformSpeedMax, Rand01()) * sign;
	plat->velocity.x =vel;

	vec3 framePos = vec3_create(0,pos.y,1.0f);
	
	Entity *frame = entity_create(Game->state.entitiesArena, ENTITY_KIND_STATIC, framePos, mat4x4_identity);
	frame->killOnRestart = True;
	frame->spriteRawTexture = &Game->texMovingPlatformFrame;
	frame->spriteSize.x = (minMaxX*2) +plat->spriteSize.x;
	r32 aspect = (r32)Game->texMovingPlatformFrame.height/Game->texMovingPlatformFrame.width;
	frame->spriteSize.y = aspect*frame->spriteSize.x;
	frame->hasSprite = True;
	frame->tilingOff = tiling_and_offset_identity;
      }
      
    }

    last = pos.y;
  }

  return last;
}


internal void game_on_physics_update(){

  AppState *appState = Game->app;

  if(Game->charges>0){

    if(Game->leftInput && Game->stickman->velocity.x >=0){

      //Game->stickman->accumulatedForce.x-=5250;
      Game->stickman->velocity.x = -Game->maxHSpeed;
      // @TODO, sfx
      if(Game->charges>0){
	Game->charges--;
	QueuedAudioBuffer *thrustSfx=audio_queue_buffer(&appState->audio, &Game->SpaceCraftThrustSfx, 1);
	for(int i=0;i<gJmpNumParticleSystems;i++){
	  if(!Game->particles[i].alive){
	    JmpParticle *ps=&Game->particles[i];
	    ps->alive = True;
	    //ps->mat = mat4x4_rotation_axis(vec3_create(0,0,1.0f), -PIE);
	    //ps->mat = mat4x4_translate(ps->mat, Game->stickman->pos.x, Game->stickman->pos.y, Game->stickman->pos.z);
	    ps->localPos = vec3_zero;
	    ps->localPos.x+=1.8f;
	    ps->localPos.y-=0.8f;
	    ps->vel = vec3_create(0,0,0);
	    ps->angles = vec3_create(0,0,0);
	    ps->age = 0;
	    ps->startScale = 0.2f;
	    ps->endScale = 1.0f;
	    ps->duration = .2f;
	    break;
	  }
	}
      }
      
    }
    if(Game->rightInput && Game->stickman->velocity.x <=0){
      //Game->stickman->accumulatedForce.x+=5250;
      Game->stickman->velocity.x = Game->maxHSpeed;
      if(Game->charges>0){
	Game->charges--;
	QueuedAudioBuffer *thrustSfx=audio_queue_buffer(&appState->audio, &Game->SpaceCraftThrustSfx, 1);
	for(int i=0;i<gJmpNumParticleSystems;i++){
	  if(!Game->particles[i].alive){
	    JmpParticle *ps=&Game->particles[i];
	    ps->alive = True;
	    //ps->mat = mat4x4_rotation_axis(vec3_create(0,0,1.0f), -PIE);
	    //ps->mat = mat4x4_translate(ps->mat, Game->stickman->pos.x, Game->stickman->pos.y, Game->stickman->pos.z);
	    ps->localPos = vec3_zero;
	    ps->localPos.x-=1.8f;
	    ps->localPos.y-=0.8f;
	    ps->vel = vec3_create(0,0,0);
	    ps->angles = vec3_create(0,0,2*PIE);
	    ps->age = 0;
	    ps->startScale = 0.2f;
	    ps->endScale = 1.0f;
	    ps->duration = .2f;
	    
	    break;
	  }
	}
      }
    }
  }
  Game->leftInput = False;
  Game->rightInput = False;
  
  CollisionInfo *col = Game->state.currPhysFrameFirstCollision;

  if(Game->state.numCollisionsCurrPhysFrame >0){
    while(col!=NULL){
      b32 done = False;
      for(int i=0;i<col->numCollisionPairs;i++){
	CollisionPair p = col->contactPoints[i];
	if(p.a->colliderOwningBody == Game->stickman || p.b->colliderOwningBody == Game->stickman){

	  Entity *floor = p.a->colliderOwningBody == Game->stickman? p.b : p.a;
	  r32 bottom = Game->stickman->colliderChain->prevPhysPos.y - Game->stickman->colliderChain->colliderHalfExtent.y;
	  r32 top = floor->physPos.y + floor->colliderHalfExtent.y;
	  if(Game->stickman->velocity.y<=0.0f && bottom >=top){
	    //@HACK, physics engine doesn't do collision resolution right now, so do it here
	    // otherwise we get inconsistent jump height due to variable amounts of penetration into the floor
	    vec3 resolvedPosition = Game->stickman->physPos;
	    resolvedPosition.y = top + Game->stickman->colliderChain->colliderHalfExtent.y;
	    // don't teleport, it messes with the interpolation, just set physpos
	    //entity_teleport(Game->stickman, resolvedPosition);
	    Game->stickman->physPos = resolvedPosition;


	    Game->stickman->velocity.y = 60;

	    QueuedAudioBuffer *jumpsfx=audio_queue_buffer(&appState->audio, &Game->JumpSfx, 1);

	    if(!floor->colliderOwningBody->used){
	      Game->charges+=Game->chargesPerJump;
	      Game->charges = Clamp(Game->charges,0,Game->maxCharges);
	      floor->colliderOwningBody->used = True;
	      floor->colliderOwningBody->spriteRawTexture = &Game->texPlatformDead;
	    }
	  }
	  done = True;
	  break;
	}
      }
      if(done) break;

      col = col->next;
    }
  }


  i32 numEnt=CountStructs(Game->state.entitiesArena, Entity);
  for(i32 i=0;i<numEnt;i++){
    Entity *en = GetStruct(Game->state.entitiesArena, Entity, i);

    if(en->kind == ENTITY_KIND_PLATFORM){
      if(en->isMovingPlatform){
	if(en->physPos.x >= gJmpPlatformMaxXOffset){

	  // force left velocity instead of just flipping current to avoid situation where platform is still beyond the bounds across multiple fixed updates
	  en->velocity.x=-(Abs(en->velocity.x));
	}else if(en->physPos.x <= -gJmpPlatformMaxXOffset){
	  en->velocity.x=Abs(en->velocity.x);
	}
      }

    }
  }
}



internal void game_update(r32 dt, AppState *appState){

  gui_on_frame_start(&Game->gui);

  if(appState->input.a.wasDownedThisFrame || appState->input.leftArrow.wasDownedThisFrame){
    Game->leftInput = True;

  }
  if(appState->input.d.wasDownedThisFrame || appState->input.rightArrow.wasDownedThisFrame){
    Game->rightInput = True;
  }


  {
    // debugging movement blurriness
    /* r32 dSpeed = 30; */
    /* if(appState->input.lshift.isPressed){ */
    /*   dSpeed = 10; */
    /* } */
    /* if(appState->input.w.isPressed){ */
    /*   Game->stickman->pos.y+=dSpeed*dt; */
    /* } */
    /* if(appState->input.s.isPressed){ */
    /*   Game->stickman->pos.y-=dSpeed*dt; */
    /* } */

    /* if(appState->input.a.isPressed){ */
    /*   Game->stickman->pos.x-=dSpeed*dt; */
    /* } */
    /* if(appState->input.d.isPressed){ */
    /*   Game->stickman->pos.x+=dSpeed*dt; */
    /* } */
  }
  
  
  Input input = appState->input;
  spc_update_gui_input(input.mousePos, input.mouse1.isPressed, input.mouse1.wasReleasedThisFrame);
  arena_clear(Game->state.frameArena);

  /* if(appState->input.s.wasDownedThisFrame){ */
  /*   Game->tempSwapInterval = Game->tempSwapInterval == 0 ? 1 : 0; */
  /*   gfx_set_swap_interval(Game->tempSwapInterval); */
  /* } */
  if(appState->input.escape.wasDownedThisFrame){
    Game->state.paused = !Game->state.paused;
  }

  if(Game->failed){
    Game->state.paused = True;
  }
  /* if(appState->input.lshift.wasDownedThisFrame){ */
  /*   QueuedAudioBuffer *kick=audio_queue_buffer(&appState->audio, &Game->KickDrum, 1); */
  /* } */
  /* if(appState->input.w.wasDownedThisFrame){ */
  /*   if(Game->TestQueued == NULL){ */
  /*     Game->TestQueued=audio_queue_buffer(&appState->audio, &Game->TestSong, 1); */
  /*   } */
  /*   else if(Game->TestQueued->copying){ */
  /*     Game->TestQueued->copying = False; */
  /*     Game->TestQueued = NULL; */
  /*   } */
  /* } */
  /* if(appState->input.q.isPressed){ */
  /*   appState->audio.testWaveFrequency+=50*dt; */
  /* } */
  /* if(appState->input.e.isPressed){ */
  /*   appState->audio.testWaveFrequency-=50*dt; */
  /* } */
  
  if(Game->state.paused) dt = 0;

  float physTimestepProgressNormalized = (Game->state.physAccumulator) / (Game->state.physTimeStep);
  physTimestepProgressNormalized = Clamp01(physTimestepProgressNormalized);





  Color clearColor = {.r = 0.722f, .g =0.682f, .b =0.576f, .a = 1};
  gfx_push_clear(&Game->state.gfxCmdList, clearColor);



  

   ////////////////////////
// ~sameer: some testing
  Color quadCol = color_white;
  Color clampCol = color_white;
  vec3 quadPos = {0.0f,0.0f,-1.0f};




  TilingAndOffset tof = {.tiling = {5,5}, .offset = {0,0}};
  //gfx_push_textured_quad(&Game->state.gfxCmdList, tof,clampCol,quadCol, &Game->texTest, 192,108, quadPos);



  //TilingAndOffset tofTest = {.tiling = {1,1}, .offset = {0,0}};
  //gfx_push_textured_quad(&Game->state.gfxCmdList, tofTest,color_white,color_white, Game->fontDefault.tex, 64,64, quadPos);


  


  r32 dif = Game->maxHeightReached - Game->stickman->pos.y;
  if(dif> Game->worldHeightInScreen*1.25f && !Game->failed){
    Game->failed = True;
    entity_teleport(Game->stickman, vec3_create(0,-10000,0));
    Game->stickman->experiencedGravity =vec3_zero;

    b32 writenew = False;
    {
      if(Game->maxHeightReached>Game->prevBest){
	writenew = True;
      }
    }
    if(writenew){
      File file=file_open(str8_lit("save.txt"), AccessFlag_Write);
      _log("New score: %f", Game->maxHeightReached);

      string8 newBestString = r32_to_string8(Game->state.frameArena, Game->maxHeightReached);

      file_write(file, newBestString);
      file_close(file);
    }
    
  }


  //@TODO, delete out of view platforms
  Game->lastPlatformPos = spc_generate_platforms(Game->stickman->pos, Game->platformSpawnOffset, Game->lastPlatformPos, Game->stickman->pos.y+100, gJmpPlatformMaxXOffset);

  // arena tests
  /* if(appState->input.space.wasDownedThisFrame){ */
  /*   PushStruct(Game->state.entitiesArena, Entity); */
  /* } */
  /* else if(appState->input.lshift.wasDownedThisFrame){ */
  /*   //PopStruct(Game->state.entitiesArena, Entity); */
  /*   arena_clear(Game->state.entitiesArena); */
  /* } */



  
  // we can walk through an arena with the same sized structs without any alignment issues, sizeof is guaranteed to be a multiple of alignof
  i32 numEnt=CountStructs(Game->state.entitiesArena, Entity);

  // interpolation, before view mat update
  for(i32 i=0;i<numEnt;i++){
    Entity *en = GetStruct(Game->state.entitiesArena, Entity, i);
    if(!en->alive) continue;
    if(en->physics && !en->staticPhysics){
      vec3 lerpedPos;
      vec_lerp(en->prevPhysPos, en->physPos,physTimestepProgressNormalized,lerpedPos, 3);
      en->pos = lerpedPos;
    }
  }



  // update view mat to follow player AFTER the player position has been interpolated
  if(Game->stickman->pos.y>=(-Game->viewMat.cr31 + Game->viewOffsetFromPlayer)){
    Game->viewMat = mat4x4_translation(0,-(Game->stickman->pos.y-Game->viewOffsetFromPlayer),Game->viewZOffset);
  }

  {
    // render bg before rendering entities
    
    mat4x4 v = mat4x4_translation(0,0,Game->viewZOffset);
    gfx_push_set_matrix(&Game->state.gfxCmdList, GFX_MATRIX_TYPE_VIEW, v);
    gfx_push_set_matrix(&Game->state.gfxCmdList, GFX_MATRIX_TYPE_PROJECTION, Game->projMatScreenCenter);
    r32 heightScale = (r32)Game->texBg.height/ Game->texBg.width;
    r32 w = Game->app->screenWidth;
    r32 h = w * heightScale;
    TilingAndOffset toff = tiling_and_offset_identity;
    toff.offset.y = -Game->viewMat.cr31*.001f;
    
    gfx_push_textured_quad(&Game->state.gfxCmdList, toff,color_white,color_white, &Game->texBg, w,h, vec3_zero,0);  
  }
  

  gfx_push_set_matrix(&Game->state.gfxCmdList, GFX_MATRIX_TYPE_VIEW, Game->viewMat);

  


  gfx_push_set_matrix(&Game->state.gfxCmdList, GFX_MATRIX_TYPE_PROJECTION, Game->projMat);
  // rendering, after view mat update
  for(i32 i=0;i<numEnt;i++){
    Entity *en = GetStruct(Game->state.entitiesArena, Entity, i);
    if(!en->alive) continue;
    if(en->kind == ENTITY_KIND_COLLIDER){

      // visualize colliders
      /* TilingAndOffset tiling = {.tiling = {1,1}, .offset = {0,0}}; */

      /* Color col = {1.0f,1.0f,1.0f,1.0f}; */
      /* if(en->colliderType == COLLIDER_TYPE_SPHERE){ */
      /* 	gfx_push_textured_quad(&Game->state.gfxCmdList, tiling,col,col, Game->texCircleCol.gfxTex, en->colliderRadius,en->colliderRadius, en->physPos); */
      /* } else if(en->colliderType == COLLIDER_TYPE_CUBOID){ */
      /* 	gfx_push_textured_quad(&Game->state.gfxCmdList, tiling,col,col, Game->texRectCol.gfxTex, en->colliderHalfExtent.x*2,en->colliderHalfExtent.y*2, en->physPos); */
      /* } */
    }

    //@NOTE we don't have transparent sorting in the graphics core
    // instead, we're manually controlling when we render an entity until we have proper transparent rendering
    if(en->kind != ENTITY_KIND_PLAYER && en->hasSprite){
      game_render_entity(en, &Game->state);
    }
  }

  
  {
    // render on top of everything
    game_render_entity(Game->stickman, &Game->state);

    for(int i=0;i<gJmpNumParticleSystems;i++){
      JmpParticle *ps = &Game->particles[i];
      if(!ps->alive) continue;

      ps->localPos = vec3_add(ps->localPos, vec3_scalef(ps->vel, dt));
      vec3 pos = Game->stickman->pos;


      pos = vec3_add(pos, ps->localPos);
      vec3 size = vec3_create(3.5f,3.5f,3.5f);
      ps->age+=dt;
      r32 nAge = (ps->age / ps->duration);
      nAge = Clamp01(nAge);
      size = vec3_scalef(size, Lerp(ps->startScale, ps->endScale, nAge));

      Color col = color_white;
      col.a = 1-(nAge*nAge);
      gfx_push_textured_quad(&Game->state.gfxCmdList, tiling_and_offset_identity,color_white,col, &Game->texThrust, size.x,size.y, pos, ps->angles.z);

      if(ps->age>=ps->duration){
	ps->alive = False;

      }
    }
  }



  {

    /* { */
    /*   // grid */
    /*   //@TODO calculate according to orthographic projection width *\/ */
    /*   // use the ui projection. */
    /*   TilingAndOffset gridTof = {.tiling = {20,20}, .offset = {0,0}}; */
    /*   Color gridCol = {0.2f,0.2f,0.2f,1.0f}; */
    /*   // use width for both, we want to maintain aspect. Let it overflow on the smaller side (usually height) */

    /*   vec3 gridPos = {0,0,-1}; */
    /*   gfx_push_textured_quad(&Game->state.gfxCmdList, gridTof,color_white,gridCol, &Game->texGrid, Game->worldWidthInScreen,Game->worldWidthInScreen, gridPos); */
    /* } */
    

  }
  



  
  physics_update(&Game->state,dt);


  Game->maxHeightReached = Max(Game->maxHeightReached, Game->stickman->pos.y);



  

  // render text on top of everything
  gfx_push_set_matrix(&Game->state.gfxCmdList, GFX_MATRIX_TYPE_PROJECTION, Game->projMatScreenCenter);
  gfx_push_set_matrix(&Game->state.gfxCmdList, GFX_MATRIX_TYPE_VIEW, Game->viewMatScreenCenter);
  vec3 scoreTextPos = {0, Game->app->halfScreenHeight-120,0.0f};
  //vec3 scoreTextPos = {-20,-20,0.0f};


  // future employers, close your eyes for this section
  i32 numDigits = 1;
  if(Game->maxHeightReached>=10) numDigits = 2;
  if(Game->maxHeightReached>=100) numDigits = 3;
  if(Game->maxHeightReached>=1000) numDigits = 4;
  if(Game->maxHeightReached>=10000) numDigits = 5;
  if(Game->maxHeightReached>=100000) numDigits = 6;
  if(Game->maxHeightReached>=1000000) numDigits = 7;
  if(Game->maxHeightReached>=10000000) numDigits = 8;
  if(Game->maxHeightReached>=100000000) numDigits = 9;
  if(Game->maxHeightReached>=1000000000) numDigits = 10;

  i32 score = Game->maxHeightReached;

  string8 scoreString;
  scoreString.ptr=arena_push(Game->state.frameArena, sizeof(u8)*(numDigits+1),1,True);
  scoreString.length = numDigits+1;

  // Writes the formatted integer into the character array
  sprintf(scoreString.ptr, "%d", score);
  scoreString.ptr[scoreString.length-1] = 'm';
  


  //r32 fwidth = font_string_width_from_pixel_size(Game->fontDefault, scoreString, jmp_FontSizeScore);
  //gfx_push_textured_quad(&Game->state.gfxCmdList, tiling_and_offset_identity,color_white,color_black, &Game->texWhite, fwidth,jmp_FontSizeScore, scoreTextPos,0);
  gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, scoreString,jmp_FontSizeScore,scoreTextPos, TEXT_ALIGN_CENTER_MIDDLE,color_white);

  string8 prevBestString = str8cat(Game->state.frameArena,str8_lit("Previous best: "), i32_to_string8(Game->state.frameArena, Game->prevBest));
  prevBestString = str8cat(Game->state.frameArena, prevBestString, str8_lit("m"));
  gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, prevBestString,20,vec3_create(scoreTextPos.x, scoreTextPos.y-30,scoreTextPos.z), TEXT_ALIGN_CENTER_MIDDLE,color_create(1,1,1,0.6f));
  



  Game->secondsSinceFpsUpdate+=dt;

  if(Game->secondsSinceFpsUpdate>=.5f){
    Game->secondsSinceFpsUpdate=0;
    Game->lastFrameTime = dt;
  }
  Color fpsColor = color_create(.8f,0.8f,0.8f,1.0f);
  if(dt>.5f){
    fpsColor = color_create(.9f,0.2f,0.2f,1.0f);
  }

  vec3 fpsPos = {-Game->app->halfScreenWidth+40, Game->app->halfScreenHeight-15,0.0f};




  char fpsNumber[16];


  sprintf(fpsNumber, "%d", (i32)(ceilf(1/Game->lastFrameTime)));

  string8 fpsLabel = str8cat(Game->state.frameArena, str8_lit("FPS: "), str8_from_cstring8(fpsNumber));


  i32 fpsLabelFontSize = 23;
  r32 fwidth = font_string_width_from_pixel_size(Game->fontDefault, fpsLabel, fpsLabelFontSize);

  gfx_push_textured_quad(&Game->state.gfxCmdList, tiling_and_offset_identity,color_white,color_create(0.0f,0.0f,0.0f,.6f), &Game->texWhite, fwidth+5,fpsLabelFontSize, fpsPos,0);
  gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, fpsLabel,fpsLabelFontSize,fpsPos, TEXT_ALIGN_CENTER_MIDDLE,fpsColor);


  /* { */

  /*   i32 stepSize = 40; */
  /*   TilingAndOffset tof = {.tiling = {1,1}, .offset = {0,0}}; */

  /*   r32 maxY = 100; */
  /*   r32 maxX = 300; */

  /*   vec3 center = {Game->app->halfScreenWidth-maxX-10,Game->app->halfScreenHeight-maxY-10,0.0f}; */


  /*   Color audioWaveColor  = color_create(0.2f,0.6f,0.2f,1.0f); */




  /*   r32 xWritePos = Game->app->audio.writePosFrames / (r32)Game->app->audio.master.sizeInFrames; */
  /*   xWritePos-=0.5f; */
  /*   xWritePos*=2.0f; */



  /*   gfx_push_textured_quad(&Game->state.gfxCmdList, tof,color_white,color_create(0.1f,0.1f,0.1f,0.8f), &Game->texWhite, maxX*2.0f,2.0f, center,0); */




  /*   i32 numBar=0; */


  /*   for(int i=0;i<Game->app->audio.master.sizeInFrames;i+=stepSize){ */

  /*     //@TODO, read other channel as well */
  /*     i16 *sample = (i16*)(Game->app->audio.master.data + Game->app->audio.master.bytesPerFrame*i); */
  /*     r32 nSample = (r32)*sample/32768.0f; */

  /*     r32 nIndex = i-((r32)Game->app->audio.master.sizeInFrames)/2.0f; */
  /*     nIndex*=2; */
  /*     nIndex/=((r32)Game->app->audio.master.sizeInFrames); */
      
  /*     r32 x = nIndex*maxX; */
  /*     r32 y = nSample*maxY; */

  /*     r32 xDrawSize = 8; */
  /*     vec3 pos = {x+xDrawSize/2.0f,y/2.0f,0.0f}; */
  /*     //pos.x+=i*numBar; */
  /*     numBar++; */
  /*     pos = vec3_add(pos,center); */
  /*     gfx_push_textured_quad(&Game->state.gfxCmdList, tof,color_white,audioWaveColor, &Game->texWhite, xDrawSize,y, pos,0); */
      
  /*   } */

  /*   //Audio write cursor, enable it for debugging, can be quite annoyingly fast for small buffers */
  /*   /\* vec3 mousePos = {center.x+xWritePos*maxX,center.y,0.0f}; *\/ */
  /*   /\* gfx_push_textured_quad(&Game->state.gfxCmdList, tof,color_white,color_create(0.9f,0.4f,0.4f,1.0f), &Game->texWhite, 5.0f,15.0f, mousePos); *\/ */


    

  /* } */

  {
    // debug strings
    /* string8 mousePosString; */
    /* mousePosString.ptr = arena_push(Game->state.frameArena, sizeof(u8)*(16), 1, True); */
    /* mousePosString.length = 16; */
    /* sprintf(mousePosString.ptr, "%d, %d, %d", Game->app->input.mousePos.x, Game->app->input.mousePos.y, Game->app->input.mousePos.z); */
    /* gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, mousePosString, vec3_create(fpsPos.x, fpsPos.y-50, fpsPos.z), TEXT_ALIGN_LEFT_TOP, color_white); */


    string8 debugString;
    /* if(Game->app->input.mouse1.isPressed){ */
    /*   debugString = str8_lit("Yes"); */
    /* } */
    /* else{ */
    /*   debugString = str8_lit("No"); */
    /* } */
    debugString.ptr = arena_push(Game->state.frameArena, sizeof(u8)*16,1,True);
    debugString.length = 16;
    /* sprintf(debugString.ptr, "Charges: %d\n", Game->charges); */
    /* gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, debugString, vec3_create(fpsPos.x, fpsPos.y-80, fpsPos.z), TEXT_ALIGN_LEFT_TOP, color_white); */
  }




  {
    // charges
    vec3 labelPos = {0, Game->app->halfScreenHeight - 15, 0};

    gfx_push_text(&Game->state.gfxCmdList, &Game->fontDefault, str8_lit("FUEL"), jmp_FontSizeDefault,labelPos, TEXT_ALIGN_CENTER_TOP, color_white);

    vec3 chargeSize = {30,30,30};
    vec3 chargeMargin = {5,0,0};
    r32 chargesWidth = chargeSize.x*Game->maxCharges + chargeMargin.x *(Game->maxCharges-1);
    vec3 chargePosStart = {labelPos.x-chargesWidth/2.0f +chargeSize.x/2.0f, labelPos.y-45,labelPos.z};

    for(int i=0;i<Game->maxCharges;i++){
      vec3 chPos = vec3_add(chargePosStart, vec3_scalef(chargeMargin, i));

      // quad position at at their center, so move it right such that they are side to side with no overlap
      chPos.x+=(chargeSize.x)*i;
      Color col = color_create(0.3f,0.3f,0.3f,1.0f);
      if(i<Game->charges){
	col = color_create(1,1,1, 1.0f);	
      }
      gfx_push_textured_quad(&Game->state.gfxCmdList, tiling_and_offset_identity,color_white,col, &Game->texThrusterFuel, chargeSize.x, chargeSize.y, chPos,0); 
    }


  }

  
  if(Game->state.paused){
    cursor_unlock();
    cursor_show();
    spc_menu();
  }
  else{
    cursor_lock();
    cursor_hide();
  }
  
  local_persist b32 firstGameLoop = True;
  if(firstGameLoop){
    _log("Space game first loop");
    firstGameLoop=False;
  }
}
internal void game_video_update(r32 dt, AppState *appState){

  gfx_exec_list_and_clear(&Game->state.gfxCmdList);


}


