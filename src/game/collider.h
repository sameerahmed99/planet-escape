#ifndef COLLIDER_CORE_INCLUDE
#define COLLIDER_CORE_INCLUDE

typedef struct CollisionPair CollisionPair;



struct CollisionPair{
  vec3 normal;
  vec3 pos;
  float depth;
  struct Entity *a, *b;
};

typedef struct CollisionInfo CollisionInfo;

struct CollisionInfo{
  CollisionInfo *next;
  i32 numCollisionPairs;
  CollisionPair *contactPoints;
} ;


typedef enum ColliderType {
  COLLIDER_TYPE_CUBOID,
  COLLIDER_TYPE_SPHERE
} ColliderType;

#endif // COLLIDER_CORE_INCLUDE
