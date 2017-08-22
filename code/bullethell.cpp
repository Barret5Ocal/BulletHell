void CreateSphereModel(game_state *GameState, real32 Radius, uint32 Rings, uint32 Sectors)
{
    model *Model = (model *)PushStruct(&GameState->Models, model);
    Model->Count = Rings * Sectors;
    Model->Vertices = (vertex *)PushArray(&GameState->Models, Rings * Sectors * 6, float);
    real32 PI_OVER_2 = GB_MATH_PI/2;
    
    float const R = 1./(float)(Rings-1);
    float const S = 1./(float)(Sectors-1);
    int r, s;
    
    vertex *Vertex = Model->Vertices;
    for(r = 0; r < Rings; r++) for(s = 0; s < Sectors; s++) 
    {
        float const y = gb_sin( -PI_OVER_2 + GB_MATH_PI * r * R );
        float const x = gb_cos(2*GB_MATH_PI * s * S) * gb_sin( GB_MATH_PI * r * R );
        float const z = gb_sin(2*GB_MATH_PI * s * S) * gb_sin( GB_MATH_PI * r * R );
        
        Vertex->Pos.x = x * Radius;
        Vertex->Pos.y = y * Radius;
        Vertex->Pos.z = z * Radius;
        
        Vertex->Norm.x = x;
        Vertex->Norm.y = y;
        Vertex->Norm.z = z;
        ++Vertex;
    }
    
    Model->ICount = Rings * Sectors * 4;
    uint32 *i = (uint32 *)PushArray(&GameState->Models, Model->ICount, uint32);
    Model->Indices = i; 
    for(r = 0; r < Rings; r++) for(s = 0; s < Sectors; s++) {
        *i++ = r * Sectors + s;
        *i++ = r * Sectors + (s+1);
        *i++ = (r+1) * Sectors + (s+1);
        *i++ = (r+1) * Sectors + s;
    }
}

void GenerateAABB(entity *Entity)
{
    uint32 Count = Entity->Model->Count;
    m4 Rotate;
    
    gb_mat4_from_quat(&Rotate, Entity->Quaternion);
    m4 Scale;
    gb_mat4_scale(&Scale, Entity->Scale);
    
    real32 MinX = 0;
    real32 MinY = 0;
    real32 MinZ = 0;
    real32 MaxX = 0;
    real32 MaxY = 0;
    real32 MaxZ = 0;
    
    for(uint32 Index = 0;
        Index < Count;
        ++Index)
    {
        vertex *Vertex = &Entity->Model->Vertices[Index];
        v3 Position = Vertex->Pos;
        v4 Pos4 = v4{Position.x, Position.y, Position.z, 1.0f};
        Pos4 =  Scale * Rotate * Pos4;
        Position = Pos4.xyz;
        if(Index == 0)
        {
            MinX = Position.x;
            MinY = Position.y;
            MinZ = Position.z;
            MaxX = Position.x;
            MaxY = Position.y;
            MaxZ = Position.z;
        }
        else 
        {
            if(Position.x < MinX) MinX = Position.x;
            if(Position.y < MinY) MinY = Position.y;
            if(Position.z < MinZ) MinZ = Position.z;
            if(Position.x > MaxX) MaxX = Position.x;
            if(Position.y > MaxY) MaxY = Position.y;
            if(Position.z > MaxZ) MaxZ = Position.z;
        }
    }
    
    v3 Min = {MinX, MinY, MinZ};
    v3 Max = {MaxX, MaxY, MaxZ};
    v3 Center;
    gb_vec3_lerp(&Center, Min, Max, 0.5f);
    v3 HalfL = Max - Center;
    Entity->Aabb = {Center, HalfL};
    
}

entity *InitEntity(game_state *GameState, uint32 Type, v3 Pos, real32 Angle, v3 Axis, v3 Scale, model *Model)
{
    entity *Entity = (entity *)PushSize(&GameState->Entities);
    *Entity = {};
    Entity->Type = Type; 
    Entity->Pos = Pos;
    Entity->Quaternion = gb_quat_axis_angle({1.0f,1.0f,1.0f}, 0);
    Entity->Scale = Scale;
    Entity->Model = Model;
    GenerateAABB( Entity);
    Entity->ID = GameState->IDCount++;
    
    return Entity; 
}

void InitLevelBlock(game_state *GameState, v3 Pos, float Angle, v3 Axis, v3 Scale, model *Model, uint32 Type)
{
    level_block *Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    //entity *Entity = (entity *)PushStruct(&GameState->Entities, entity);
    entity *Entity = InitEntity(GameState, BOX, Pos, Angle, Axis, Scale, Model);
    
}

model *SeekModel(memory_arena *Models, int Index)
{
    uint8 *Seek = (uint8 *)Models->Memory;
    for(uint32 i = 0; 
        i < Index; 
        ++i)
    {
        model *Model = (model *)Seek;
        Seek += sizeof(model);
        int VCount = Model->Count; 
        Seek += VCount * sizeof(vertex);
        if(Model->ICount)
            Seek += Model->ICount * sizeof(uint32);
    }
    
    return (model *)Seek; 
}

void InitBullet(game_state *GameState)
{
    player *Player  = &GameState->Player;
    camera *Camera = &Player->Camera; 
    dynamic_arena *Bullets = &GameState->Bullets;
    
    bullet *Bullet = (bullet *)PushSize(Bullets);
    *Bullet = {};
    Bullet->Entity = InitEntity(GameState, BULLET, -(Player->Entity->Pos + (Camera->Eye * 3.0f)), 0, {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}, SeekModel(&GameState->Models, 1));
    Bullet->Direction = Camera->Eye;
    Bullet->Speed = 30;
    Bullet->ID = GameState->IDCountB++;
}

void DestroyBullet(game_state *GameState, bullet* Bullet)
{
    RemoveSize(&GameState->Entities, Bullet->Entity->ID);
    RemoveSize(&GameState->Bullets, Bullet->ID);
}

scene_layout *LoadSceneLayout(game_state *GameState)
{
    AllocateArena(&GameState->SceneArena, Megabyte(2));
    GameState->Scene1 =(scene_layout *)PushStruct(&GameState->SceneArena, scene_layout);
    scene_layout *Scene = GameState->Scene1;
    
    Scene->Count = 5;
    Scene->Blocks = (level_block *)(Scene + 1);
    
    InitLevelBlock(GameState, {0.0f, -3.0f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, {100.0f, 1.0f, 100.0f}, (model *)GameState->Models.Memory, LEVEL_BLOCK);
    
    InitLevelBlock(GameState, {47.5f, 2.5f, 47.5f}, 0, {1.0f, 1.0f, 1.0f}, {5.0f, 10.0f, 5.0f}, (model *)GameState->Models.Memory, LEVEL_BLOCK);
    
    InitLevelBlock(GameState, {-47.5f, 2.5f, 47.5f}, 0, {1.0f, 1.0f, 1.0f}, {5.0f, 10.0f, 5.0f}, (model *)GameState->Models.Memory, LEVEL_BLOCK);
    
    InitLevelBlock(GameState, {47.5f, 2.5f, -47.5f}, 0, {1.0f, 1.0f, 1.0f}, {5.0f, 10.0f, 5.0f}, (model *)GameState->Models.Memory, LEVEL_BLOCK);
    
    InitLevelBlock(GameState, {-47.5f, 2.5f, -47.5f}, 0, {1.0f, 1.0f, 1.0f}, {5.0f, 10.0f, 5.0f}, (model *)GameState->Models.Memory, LEVEL_BLOCK);
    
    return Scene; 
}

void Setup(game_state *GameState)
{
    
    memory_arena *Models = &GameState->Models;
    
    AllocateArena(Models, Megabyte(2));
    model *Cube = (model *)PushStruct(Models, model);
    Cube->Count = 36; 
    Cube->Vertices = (vertex *)PushArray(Models, Cube->Count, vertex);
    //vertex vertices[]
    
    vertex Vertices[] = {
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
        
        {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
        {0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
        {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
        {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
        {0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
        {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
        
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
        {0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
        {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
        {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
        
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
        {0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
        {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
        {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f}
    };
    
    for(uint32 Index = 0;
        Index < Cube->Count;
        ++Index)
    {
        vertex *Vertex = Cube->Vertices + Index;
        *Vertex = Vertices[Index];
    }
    Cube->ICount = 0; 
    
    CreateSphereModel(GameState, 1.0f, 20, 20);
    
    vertex SquareV[] =
    {
        {-0.5f, 0.5f, 0.0f, 0.0f, -1.0f,  0.0f},
        {0.5f, 0.5f, 0.0f, 0.0f, -1.0f,  0.0f},
        {-0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f},
        {0.5f, 0.5f, 0.0f, 0.0f, -1.0f,  0.0f},
        {-0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f},
        {0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f},
    };
    model *Square = (model *)PushStruct(Models, model);
    Square->Count = 6; 
    Square->Vertices = (vertex *)PushArray(Models, Cube->Count, vertex);
    for(uint32 Index = 0;
        Index < Square->Count;
        ++Index)
    {
        vertex *Vertex = Square->Vertices + Index;
        *Vertex = SquareV[Index];
    }
    Square->ICount = 0; 
    
    //AllocateArena(&GameState->Entities, Megabyte(2));
    AllocateDynamic(&GameState->Entities, Megabyte(2), sizeof(entity));
    
    //GameState->Player.Entity = (entity *)PushStruct(&GameState->Entities, entity);
    GameState->Player.Entity = InitEntity(GameState, PLAYER, {0.0f, 0.0f, -10.0f}, 0, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, (model *)GameState->Models.Memory);
    //(entity *)PushSize(&GameState->Entities);
    GameState->Player.Camera.Eye = {0.0f, 0.0f, 1.0f};
    // TODO(Barret5Ocal): pretend its a cube for now. make player model later
    GameState->Player.Speed  = 10.0f;
    GameState->Player.Entity->Model = 0;
    
    AllocateArena(&GameState->Collisions, Megabyte(2));
    LoadSceneLayout(GameState);
    
    AllocateDynamic(&GameState->Bullets, Megabyte(2), sizeof(bullet));
    
    AllocateArena(&GameState->Velocities, Kilobyte(5));
}

void MovePlayer(game_state *GameState, input *Input, float dt)
{
    velocity *Velocity = (velocity *)PushStruct(&GameState->Velocities, velocity); 
    *Velocity = {}; 
    
    real32 Speed = 10.0f; 
    player *Player  = &GameState->Player;
    camera *Camera = &Player->Camera; 
    Velocity->Entity = Player->Entity;  
    
    v3 Right; 
    gb_vec3_cross(&Right, Camera->Eye, {0.0f, 1.0f, 0.0f});
    
    v3 ZMove = (Input->MoveVertical * Camera->Eye);
    v3 XMove = (-(Input->MoveHorizontal * Right));
    v3 NullY = v3{1.0f, 0.0f, 1.0f};
    real32 Increment = dt * Speed;
    Velocity->Velocity += (ZMove + XMove) * NullY * Increment;
    
    // TODO(Barret5Ocal): Aim Acceleration??? 
    real32 CamSpeed = 10.0f;
    static real32 AngleH = 0.0f;
    static real32 AngleV = 0.0f;
    AngleH += Input->MouseMove.x * dt * CamSpeed;
    AngleV += Input->MouseMove.y * dt * CamSpeed;
    AngleH += Input->CameraHorizontal * dt * CamSpeed * 20.0f;
    AngleV += Input->CameraVertical * dt * CamSpeed * 20.0f;
    
    Camera->Eye.z = gb_cos(gb_to_radians(AngleH));
    Camera->Eye.x = gb_sin(gb_to_radians(AngleH));
    Camera->Eye.y = gb_sin(gb_to_radians(AngleV));
    
    
    // NOTE(Barret5Ocal): Gravity 
    v3 Gravity = {0.0f, 1.0f, 0.0f};
    //Velocity += Gravity;
    
    Player->Entity->Velocity = Velocity->Velocity;
    
}

void MoveBullets(game_state *GameState, float dt)
{
    bullet *Bullet = (bullet *)GameState->Bullets.Memory; 
    
    for(uint32 Amount = 0;
        Amount < GameState->Bullets.AmountStored;
        ++Amount)
    {
        while(!Bullet->ID)
            ++Bullet;
        
        velocity *Velocity = (velocity *)PushStruct(&GameState->Velocities, velocity); 
        *Velocity = {}; 
        Velocity->Entity = Bullet->Entity;
        
        
        Velocity->Velocity = -(Bullet->Direction * Bullet->Speed * dt); 
        Bullet->Entity->Velocity = Velocity->Velocity; 
        ++Bullet;
    }
}

int32 AabbIntersection(aabb AABB1, aabb AABB2)
{
    real32 MinX1 = AABB1.centre.x - AABB1.half_size.x;
    real32 MinY1 = AABB1.centre.y - AABB1.half_size.y;
    real32 MinZ1 = AABB1.centre.z - AABB1.half_size.z;
    real32 MaxX1 = AABB1.centre.x + AABB1.half_size.x;
    real32 MaxY1 = AABB1.centre.y + AABB1.half_size.y;
    real32 MaxZ1 = AABB1.centre.z + AABB1.half_size.z;
    
    real32 MinX2 = AABB2.centre.x - AABB2.half_size.x;
    real32 MinY2 = AABB2.centre.y - AABB2.half_size.y;
    real32 MinZ2 = AABB2.centre.z - AABB2.half_size.z;
    real32 MaxX2 = AABB2.centre.x + AABB2.half_size.x;
    real32 MaxY2 = AABB2.centre.y + AABB2.half_size.y;
    real32 MaxZ2 = AABB2.centre.z + AABB2.half_size.z;
    
    if(MinX1 >= MaxX2) 
        return 1;
    if(MinY1 >= MaxY2) 
        return 1;
    if(MinZ1 >= MaxZ2)
        return 1;
    
    if(MinX2 >= MaxX1) 
        return 1;
    if(MinY2 >= MaxY1)
        return 1;
    if(MinZ2 >= MaxZ1) 
        return 1;
    
    return 0;
}

struct collision
{
    entity *Entity1;
    aabb AABB1; 
    
    entity *Entity2;
    aabb AABB2;
};

// TODO(Barret5Ocal): Several things. Raytracing, debug graphics. I also want to try to do the velocity buffer again. I will still need to store the velocity in the entity due to momentum, but if I only test collision between objects that are moving and all objects, it might reduce the number of loops I do.
void TestCollision(dynamic_arena *Entities,  memory_arena *Collisions, memory_arena *Velocities)
{
    ResetArena(Collisions);
    
    velocity *Velocity = (velocity *)Velocities->Memory; 
    for(uint32 Index = 0;
        Index < Velocities->Used / sizeof(velocity);
        ++Index)
    {
        
        entity *Entity1 = (Velocity +  Index)->Entity;
        
        
        entity *Entity2 = (entity *)Entities->Memory; 
        for (uint32 Index2 = 0;
             Index2 < Entities->AmountStored;
             ++Index2)
        {
            while(!Entity2->ID)
                ++Entity2;
            
            if(Entity1->ID != Entity2->ID)
            {
                aabb AABB1 = Entity1->Aabb; 
                aabb AABB2 = Entity2->Aabb;
                AABB1.centre += Entity1->Pos + Entity1->Velocity; 
                AABB2.centre += Entity2->Pos + Entity2->Velocity; 
                
                if(!AabbIntersection(AABB1, AABB2))
                {
                    collision *Collision = (collision *)PushStruct(Collisions, collision);
                    
                    Collision->Entity1 = Entity1;
                    Collision->Entity2 = Entity2; 
                    Collision->AABB1 = AABB1; 
                    Collision->AABB2 = AABB2; 
                }
                else 
                {
                    
                }
            }
            ++Entity2;
        }
        
    }
    
    
}

void ResolveCollision(collision *Collisions, int32 CollisionSize, game_state *GameState)
{
    
    dynamic_arena *Bullets = &GameState->Bullets;
    dynamic_arena *Entities = &GameState->Entities;
    
    // TODO(Barret5Ocal): finish this. i making 2 collision struct for each collision so make sure that you only modify one entity per collision
    for(uint32 Index = 0;
        Index < CollisionSize;
        ++Index)
    {
        collision *Collision = Collisions + Index;
        
        entity *Entity = Collision->Entity1;
        entity *Other = Collision->Entity2;
        aabb AABB1 = Collision->AABB1; 
        aabb AABB2 = Collision->AABB2; 
        switch(Entity->Type)
        {
            case LEVEL_BLOCK:
            {
                
            }break;
            case PLAYER:
            {
                switch(Other->Type)
                {
                    case LEVEL_BLOCK:
                    {
                        //v3 Reflect = {};
                        
                        //gb_vec3_reflect(&Reflect, Entity->Velocity, );
                        Entity->Velocity = {};
                    }break;
                    case BULLET:
                    {
                        // TODO(Barret5Ocal): Deal Damage to player
                    }break;
                    default:
                    {
                        InvalidCodePath;
                    }break;
                }
            }break;
            case BULLET:
            {
                switch(Other->Type)
                {
                    case LEVEL_BLOCK:
                    {
                        bullet *Bullet = (bullet *)Bullets->Memory;
                        
                        for(uint32 Amount = 0;
                            Amount < Bullets->AmountStored;
                            ++Amount)
                        {
                            while(!Bullet->ID)
                                ++Bullet;
                            
                            if(Bullet->Entity->ID == Entity->ID)
                            {
                                DestroyBullet(GameState, Bullet);
                                break;
                            }
                            ++Bullet;
                        }
                        
                    }break;
                    case PLAYER:
                    {
                        // TODO(Barret5Ocal): make sure bullets don't disappear if they touch the player that fired them
                        bullet *Bullet = (bullet *)Bullets->Memory;
                        for(uint32 Amount = 0;
                            Amount < Bullets->AmountStored;
                            ++Amount)
                        {
                            while(!Bullet->ID)
                                ++Bullet;
                            
                            if(Bullet->Entity->ID == Entity->ID)
                            {
                                DestroyBullet(GameState, Bullet);
                                break;
                            }
                            ++Bullet;
                        }
                        
                    }break;
                    case BULLET:
                    {
                        
                    }break;
                    default:
                    {
                        InvalidCodePath;
                    }break;
                }break;
            }
            default:
            {
                InvalidCodePath;
            }break;
        }
    }
    
    
}

void ApplyVelocity(game_state *GameState)
{
    entity *Entity = (entity *)GameState->Entities.Memory;
    for(uint32 Amount = 0;
        Amount < GameState->Entities.AmountStored;
        ++Amount)
    {
        while(!Entity->ID)
            ++Entity;
        
        Entity->Pos += Entity->Velocity;
        ++Entity;
    }
    
}

void LauchBullets(game_state *GameState, input *Input)
{
    if (Input->RightTrigger)
    {
        InitBullet(GameState);
    }
}

void RemoveBulletsOutofRange(game_state *GameState, dynamic_arena *Bullets, dynamic_arena *Entities,  real32 Range)
{
    bullet *Bullet = (bullet *)Bullets->Memory;
    for(uint32 Amount = 0;
        Amount < Bullets->AmountStored;
        ++Amount)
    {
        while(!Bullet->ID)
            ++Bullet;
        
        ++Bullet->AliveTime; 
        if(Bullet->Entity->Pos.x > Range || Bullet->Entity->Pos.x < -Range ||Bullet->Entity->Pos.y > Range || Bullet->Entity->Pos.y < -Range ||Bullet->Entity->Pos.z > Range || Bullet->Entity->Pos.z < -Range)
        {
            DestroyBullet(GameState, Bullet);
        }
        ++Bullet;
    }
    
}

void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    ResetArena(&GameState->Velocities);
    MovePlayer(GameState, Input, dt);
    LauchBullets(GameState, Input);
    MoveBullets(GameState, dt);
    
    TestCollision(&GameState->Entities, &GameState->Collisions, &GameState->Velocities);
    ResolveCollision((collision *)GameState->Collisions.Memory, GameState->Collisions.Used/sizeof(collision), GameState);
    
    ApplyVelocity(GameState);
    RemoveBulletsOutofRange(GameState, &GameState->Bullets, &GameState->Entities, 200.0f);
    
    render_setup *Setup = (render_setup *)PushStruct(RenderBuffer, render_setup);
    *Setup = {};
    Setup->CameraPos = GameState->Player.Entity->Pos;
    Setup->ViewDir = GameState->Player.Camera.Eye;
    
    entity *Entity = (entity *)GameState->Entities.Memory;
    for(uint32 Amount = 0;
        Amount < GameState->Entities.AmountStored;
        ++Amount)
    {
        while(!Entity->ID)
            ++Entity;
        
        if(Entity->Model)
        {
            render_element *Element = (render_element *)PushStruct(RenderBuffer, render_element);
            
            Element->Scale =  Entity->Scale;
            Element->Position =  Entity->Pos;
            Element->Quaternion =  Entity->Quaternion;
            Element->Material = 
            {
                {1.0f, 0.5f, 0.31f},
                {1.0f, 0.5f, 0.31f},
                {0.5f, 0.5f, 0.5f},
                32.0f
            };
            Element->Model = Entity->Model;
            ++Setup->Count;
        }
        ++Entity;
    }
    
    if(Input->Debug)
        int i = 0;
    
}

