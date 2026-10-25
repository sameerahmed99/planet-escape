#ifndef ENTITY_CORE_INCLUDE
#define ENTITY_CORE_INCLUDE




typedef enum EntityKind {
  ENTITY_KIND_PLAYER,
  ENTITY_KIND_CAMERA,
  ENTITY_KIND_COLLIDER,
  ENTITY_KIND_STATIC,
  //////////////////////////////////////////////////////////////////////////
  // ~sameer: each game needs to implement this before this file is included
  ACTIVE_GAME_ENTITY_KINDS,

  
} EntityKind;



typedef struct Entity Entity;

typedef u32 EntityId;
typedef u16 EntityGeneration;

global const EntityId ENTITY_ID_NULL = 0xFFFFFFFF;



struct Entity{
  string8 debugString;
  EntityKind kind;
  b32 hasSprite;
  Texture *spriteRawTexture;
  TilingAndOffset tilingOff;
  vec3 spriteSize;


  b32 physics;
  b32 isPhysBody;
  b32 staticPhysics;
  b32 obeyGravity;


  // physics
  // we use this instead of a single gravity value, since the game will have varying gravity based on where you are and what you are on
  vec3 experiencedGravity;
  r32 mass;

  PhysFlags physFlags;

  vec3 accumulatedForce;

  b32 hasCollider;
  Entity *colliderChain;
  Entity *colliderOwningBody;
  EntityGeneration colliderChainGeneration;
  ColliderType colliderType;

  vec3 colliderHalfExtent;
  r32 colliderRadius;
  
  

  


    // remember how matrices work, each column represents the basis vector
  // so to get the right direction of the entity, use the first column
  // for the up direction, the second
  // for the forward, the 3rd. The 4th is to facilitate having a single matrix for the three types of transformations (scale, rot, pos)
  // identity matrix also makes sense this way, the first column (the x axis coumn) has its x value set to 1, the second column (the y column) has its y value set to 1 and so on
  vec3 pos;
  mat4x4 orientation;
  mat4x4 worldTransform;
  mat4x4 localTransform;

  // 2D rotation
  r32 zRot;

  vec3 prevPhysPos;
  vec3 physPos;
  vec3 velocity;
  mat4x4 prevPhysOrientation;
  mat4x4 physOrientation;
  
  b32 alive;
  EntityId id;
  EntityGeneration generation;


 //////////////////////////////////////////////////////////////////////////
// ~sameer: each game needs to implement this before this file is included
  ACTIVE_GAME_ENTITY_DATA;
};

b32 entity_get_by_id(EntityId id, EntityGeneration generation, Arena *arena, Entity **found);

Entity *entity_create(Arena *arena, EntityKind kind, vec3 pos, mat4x4 orientation);
Entity *entity_create_sphere_collider(Arena *arena,Entity *owner, r32 radius, PhysFlags flags);
Entity *entity_create_cuboid_collider(Arena *arena,Entity *owner, vec3 halfExtent, PhysFlags flags);

void entity_teleport(Entity *en, vec3 pos);

#endif // ENTITY_CORE_INCLUDE
