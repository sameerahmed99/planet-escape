#ifndef PHYSICS_CORE_INCLUDE
#define PHYSICS_CORE_INCLUDE





/* struct Collider{ */
/*   Collider *next; */
/*   Collider *prev; */

/*   ColliderType type; */
/*   r32 radius; */
/*   vec3 bound; */
/*   vec3 center; */
/*   mat4x4 orientation; */
/* }; */

/* typedef struct PhysBody { */
/*   r32 mass; */

/*   b32 staticPhysics; */
/*   b32 gravity; */
/*   b32 positionConstraint[3]; */
/*   b32 rotationConstraint[3]; */

/* } PhysBody; */




void phys_init(GameState *state);
void physics_update(GameState *gameState, float dt);
b32 phys_check_entity_collision(Entity *a, Entity *b, CollisionInfo *out_info, Arena *arena);
b32 phys_sphere_to_sphere(vec3 pos1, r32 r1, vec3 pos2, r32 r2, CollisionInfo *info,Arena *arena);
b32 phys_cuboid_to_cuboid(Entity *colA, Entity *colB, CollisionInfo *out_info, Arena *arena);
void phys_sync_colliders_with_entity(Entity *en);
#endif // PHYSICS_CORE_INCLUDE
