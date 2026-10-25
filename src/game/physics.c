




void phys_sync_colliders_with_entity(Entity *en){
  if(en->hasCollider){
    Entity *col = en;
    while(col->colliderChain!=NULL){
      col->colliderChain->physPos = col->physPos;
      col->colliderChain->prevPhysPos = col->prevPhysPos;


      col->colliderChain->physOrientation = col->physOrientation;
      col->colliderChain->prevPhysOrientation = col->prevPhysOrientation;

      col = col->colliderChain;
    }
  }
}
void phys_sync_all_colliders_with_entity(GameState *state){
  mi num = CountStructs(state->entitiesArena, Entity);

  for(i32 i=0;i<num;i++){
    Entity *en = GetStruct(state->entitiesArena, Entity, i);
    if(!en->alive) continue;
    phys_sync_colliders_with_entity(en);
  }
}

void phys_init(GameState *state){
  phys_sync_all_colliders_with_entity(state);
}


void physics_integrate(GameState *state){


  Arena *scratch = state->scratchArena;
  TempArena temp = arena_temp_begin(scratch);
  // apply forces to velocity
  mi num = CountStructs(state->entitiesArena, Entity);
  for(i32 i=0;i<num;i++){
    Entity *en = GetStruct(state->entitiesArena, Entity, i);
    if(!en->alive) continue;
    if(!en->physics) continue;
    if(en->staticPhysics) continue;
    if(en->obeyGravity){
      en->velocity = vec3_add(en->velocity, vec3_scalef(en->experiencedGravity, state->physTimeStep));
    }

    if(!vec3_match(en->accumulatedForce, vec3_zero)){

      vec3 accel = vec3_scalef(en->accumulatedForce, (1.0f / en->mass) * state->physTimeStep);

      en->velocity = vec3_add(en->velocity, accel);
      en->accumulatedForce = vec3_zero;
    }
  }



  // update positions
  for(i32 i=0;i<num;i++){
    Entity *en = GetStruct(state->entitiesArena, Entity, i);
    if(!en->alive) continue;
    if(!en->physics) continue;
    if(en->isPhysBody){
      en->prevPhysPos = en->physPos;



      if(en->staticPhysics){

      }
      else{
	vec3 v;
	float dt = state->physTimeStep;
	vec_scalef(en->velocity, dt,v,3);

	vec3 pos;
	vec_add(en->physPos, v, pos, 3);
	en->physPos = pos;


      }

      phys_sync_colliders_with_entity(en);
    }

    
  }

  // resolve collisions, set post collision velocities and positions
  for(i32 i=0;i<num;i++){
    // set up colliders and also render them in the game's main update loop
    // constraints, box to box collision with rotations etc
    Entity *en = GetStruct(state->entitiesArena, Entity, i);
    if(!en->alive) continue;
    
    if(!en->hasCollider) continue;


    //@explanation, start from i to avoid testing entity pair that has already been tested.
    // all entities between 0 to i have already testing against entity j. 
    for(i32 j=i;j<num;j++){
      if(i==j) continue;
      Entity *en2 = GetStruct(state->entitiesArena, Entity, j);
      if(!en2->alive) continue;
      if(!en2->hasCollider) continue;

 
      CollisionInfo info;
      if(phys_check_entity_collision(en,en2, &info, temp.arena)){





	CollisionInfo *newInf=PushStruct(state->physFrameArena,CollisionInfo);


	*newInf = info;

	if(state->numCollisionsCurrPhysFrame == 0){
	  state->currPhysFrameFirstCollision = newInf;
	}
	else{
	  state->currPhysFrameLastCollision->next=newInf;

	}


	state->currPhysFrameLastCollision = newInf;

	





	for(int c=0;c<info.numCollisionPairs;c++){

	  CollisionPair *pair= PushStruct(state->physFrameArena, CollisionPair);
	  *pair = info.contactPoints[c];

	  if(c==0){
	    newInf->contactPoints = pair;
	  }
	}

	
	state->numCollisionsCurrPhysFrame++;
      }
    }
  }

  // @TODO, implement collision resolution when a game needs it
  // for now, the game i'm currently working on doesn't need it and can implement its own simple resolution using the collision info list
  


  arena_temp_end(temp);


}




void physics_update(GameState *state, float dt){


  state->numCollisionsCurrPhysFrame=0;
  state->currPhysFrameFirstCollision=NULL;
  state->currPhysFrameLastCollision=NULL;
  state->physAccumulator+=dt;

  while(state->physAccumulator >=state->physTimeStep){
    arena_clear(state->physFrameArena);

    state->physAccumulator-=state->physTimeStep;
    state->physAccumulator = Max(0,state->physAccumulator);
    physics_integrate(state);



    game_on_physics_update();
  }

  
}
b32 phys_check_entity_collision(Entity *a, Entity *b, CollisionInfo *out_info, Arena *arena){

  out_info->numCollisionPairs = 0;
  out_info->contactPoints = NULL;

  //@TODO, generation check in case collider entity died for some reason


  //@TODO, multiple collision points
  if(a->colliderChain->colliderType == COLLIDER_TYPE_SPHERE){
    if(b->colliderChain->colliderType == COLLIDER_TYPE_SPHERE){

      if(phys_sphere_to_sphere(a->colliderChain->physPos, a->colliderChain->colliderRadius,b->colliderChain->physPos,b->colliderChain->colliderRadius, out_info, arena)){


	return True;
      }
      else{
	return False;
      }
    }
    else if(b->colliderChain->colliderType == COLLIDER_TYPE_CUBOID){

    }
  }
  else if(a->colliderChain->colliderType == COLLIDER_TYPE_CUBOID){
    if(b->colliderChain->colliderType == COLLIDER_TYPE_SPHERE){

    }
    else if(b->colliderChain->colliderType == COLLIDER_TYPE_CUBOID){
      if(phys_cuboid_to_cuboid(a->colliderChain,b->colliderChain,out_info, arena)){
	return True;
      }
    }
  }

  

  return False;
}


b32 phys_sphere_to_sphere(vec3 pos1, r32 r1, vec3 pos2, r32 r2, CollisionInfo *info, Arena *arena){

  r32 r = r1+r2;
  b32 x = (pos1.x - pos2.x) <= r;
  b32 y = (pos1.y - pos2.x) <= r;
  b32 z = (pos1.y - pos2.x) <= r;

  if(x || y || z){
    CollisionPair *pair= PushStruct(arena, CollisionPair);
    vec3 dir;
    vec_sub(pos2,pos1,dir,3);

    vec3 add;
    vec_scalef(dir,r1,add,3);

    vec3 colPos;
    vec_add(pos1, add, colPos, 3);

    pair->pos = colPos;
    if(info->numCollisionPairs == 0){
      info->contactPoints= pair;
    }
    info->numCollisionPairs++;
    return True;
  }

  return False;
}


b32 phys_cuboid_to_cuboid(Entity *colA, Entity *colB, CollisionInfo *out_info, Arena *arena){

  vec3 pos1; vec3 halfExtent1; mat4x4 orientation1; vec3 pos2; vec3 halfExtent2; mat4x4 orientation2;
  pos1 = colA->physPos;
  pos2 = colB->physPos;
  halfExtent1 = colA->colliderHalfExtent;
  halfExtent2 = colB->colliderHalfExtent;
  orientation1 = colA->physOrientation;
  orientation2 = colB->physOrientation;
  
  b32 aligned = MemoryIsMatchStruct(&orientation1, &mat4x4_identity) && MemoryIsMatchStruct(&orientation2, &mat4x4_identity);
  out_info->numCollisionPairs = 0;
  out_info->contactPoints = NULL;
  if(aligned){

    b32 side = ((pos1.x - halfExtent1.x) <= (pos2.x + halfExtent2.x)) && ( (pos1.x + halfExtent1.x) >= (pos2.x - halfExtent2.x));
    b32 top = ((pos1.y - halfExtent1.y) <= (pos2.y + halfExtent2.y)) && ( (pos1.y + halfExtent1.y) >= (pos2.y - halfExtent2.y));
    b32 front = ((pos1.z - halfExtent1.z) <= (pos2.z + halfExtent2.z)) && ( (pos1.z + halfExtent1.z) >= (pos2.z - halfExtent2.z));

    if(side && top && front){
      CollisionPair *pair= PushStruct(arena, CollisionPair);
      pair->a = colA;
      pair->b = colB;
      //@TODO, multiple collision pairs with proper values
      
      /* vec3 dir; */
      /* vec_sub(pos2,pos1,dir,3); */

      /* vec3 add; */
      /* vec_scalef(dir,r1,add,3); */

      /* vec3 colPos; */
      /* vec_add(pos1, add, colPos, 3); */

      /* pair->pos = colPos; */

      if(out_info->numCollisionPairs == 0){
	out_info->contactPoints= pair;
      }
      out_info->numCollisionPairs++;
      return True;
    }
    return False;
  }
  else{
    Assert("not implemented");
  }
  return False;
}


