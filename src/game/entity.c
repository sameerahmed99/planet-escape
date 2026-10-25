


//@HACK, handle this more elegantly, move this declaration from physics.h and here to another common file
void phys_sync_colliders_with_entity(Entity *en);



b32 entity_get_by_id(EntityId id, EntityGeneration generation, Arena *arena, Entity **found){
  mi num = CountStructs(arena, Entity);
  *found = 0;
  for(i32 i=0;i<num;i++){
    Entity *en = GetStruct(arena, Entity, i);

    if(en->id == id){
      if(en->generation !=generation) {
	return False;
      }
      *found = en;
      return True;
    }
  }

  return False;
}

Entity *entity_create(Arena *arena, EntityKind kind, vec3 pos, mat4x4 orientation){

  i32 numEnt=CountStructs(arena, Entity);
  Entity *en;

  b32 found = False;
  for(int i=0;i<numEnt;i++){
    Entity *existing = GetStruct(arena, Entity, i);
    if(!existing->alive){
      en = existing;
      MemorySet(en,0, sizeof(Entity));
      found = True;
      break;
    }
  }

  if(!found){
    en=PushStruct(arena,Entity);
  }

  en->kind = kind;
  en->pos = pos;
  en->physPos = pos;
  en->prevPhysPos = pos;
  en->orientation = orientation;
  en->physOrientation = orientation;
  en->prevPhysOrientation = orientation;
  en->alive = True;

  return en;
}

Entity *entity_create_collider(Arena *arena,Entity *owner, PhysFlags flags){

  
  Entity *en=PushStruct(arena,Entity);
  en->kind = ENTITY_KIND_COLLIDER;
  en->physics = True;
  en->physFlags = flags;

  en->colliderOwningBody = owner;

  en->alive = True;
  return en;
}
Entity *entity_create_sphere_collider(Arena *arena,Entity *owner, r32 radius, PhysFlags flags){
  Entity *en=entity_create_collider(arena,owner, flags);
  en->colliderType = COLLIDER_TYPE_SPHERE;
  en->colliderRadius = radius;
  en->physics = True;
  return en;
}
Entity *entity_create_cuboid_collider(Arena *arena,Entity *owner, vec3 halfExtent, PhysFlags flags){
  Entity *en=entity_create_collider(arena,owner,flags);
  en->colliderType = COLLIDER_TYPE_CUBOID;
  en->colliderHalfExtent = halfExtent;
  en->physics = True;
  return en;
}
void entity_teleport(Entity *en, vec3 pos){
  en->pos = pos;
  en->physPos = pos;
  en->prevPhysPos = pos;
  if(en->isPhysBody){
    phys_sync_colliders_with_entity(en);
  }

}
