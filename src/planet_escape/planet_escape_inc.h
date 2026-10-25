#ifndef BIKER_INCLUDE
#define BIKER_INCLUDE



#define ACTIVE_GAME_ENTITY_DATA			\
  b32 used;					\
  b32 isMovingPlatform;				\
  b32 killOnRestart; 

#define ACTIVE_GAME_ENTITY_KINDS \
  ENTITY_KIND_PLATFORM

#include "../game/game_inc.h"
#include "planet_escape.c"





#endif // BIKER_INCLUDE
