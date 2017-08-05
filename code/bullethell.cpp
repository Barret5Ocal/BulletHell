void GenerateAABB(entity *Entity)
{
    uint32 Count = Entity->Model->Count;
    m4 Rotate;
    gb_mat4_rotate(&Rotate, Entity->Axis, gb_to_radians(Entity->Angle));
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

void InitLevelBlock(game_state *GameState, v3 Pos, float Angle, v3 Axis, v3 Scale, model *Model, uint32 Type)
{
    level_block *Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    entity *Entity = (entity *)PushStruct(&GameState->Entities, entity);
    Block->Entity = Entity; 
    Block->Type = BOX; 
    Entity->Pos = Pos;
    Entity->Angle = Angle;
    Entity->Axis = Axis;
    Entity->Scale = Scale;
    Entity->Model = Model;
    Entity->Type = Type;
    GenerateAABB(Entity);
    
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
    AllocateArena(&GameState->Entities, Megabyte(2));
    GameState->Player.Entity = (entity *)PushStruct(&GameState->Entities, entity);
    GameState->Player.Entity->Pos = {0.0f, 0.0f, -10.0f};
    GameState->Player.Camera.Eye = {0.0f, 0.0f, 1.0f};
    // TODO(Barret5Ocal): pretend its a cube for now. make player model later
    GameState->Player.Entity->Model = (model *)GameState->Models.Memory;
    GameState->Player.Entity->Angle = 0;
    GameState->Player.Entity->Axis = {1.0f, 1.0f, 1.0f};
    GameState->Player.Entity->Scale = {1.0f, 1.0f, 1.0f};
    GameState->Player.Entity->Type = PLAYER;
    GenerateAABB(GameState->Player.Entity);
    
    LoadSceneLayout(GameState);
    
    AllocateArena(&GameState->Collisions, Megabyte(2));
}

void MovePlayer(game_state *GameState, input *Input, float dt)
{
    v3 Velocity = {}; 
    
    real32 Speed = 10.0f; 
    player *Player  = &GameState->Player;
    camera *Camera = &Player->Camera; 
    
    v3 Right; 
    gb_vec3_cross(&Right, Camera->Eye, {0.0f, 1.0f, 0.0f});
    
#if 0
    if(Input->MoveVertical > 0) Velocity += (Camera->Eye * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    if(Input->MoveVertical < 0) Velocity -= (Camera->Eye * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    if(Input->MoveHorizontal > 0) Velocity -= (Right  * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    if(Input->MoveHorizontal < 0) Velocity += (Right  * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
#else 
    v3 ZMove = (Input->MoveVertical * Camera->Eye);
    v3 XMove = (-(Input->MoveHorizontal * Right));
    v3 NullY = v3{1.0f, 0.0f, 1.0f};
    real32 Increment = dt * Speed;
    Velocity += (ZMove + XMove) * NullY * Increment;
#endif 
    
    // TODO(Barret5Ocal): Aim Acceleration??? 
    real32 CamSpeed = 10.0f;
    static real32 AngleH = 0.0f;
    static real32 AngleV = 0.0f;
    AngleH += Input->MouseMove.x * dt * CamSpeed;
    AngleV += Input->MouseMove.y * dt * CamSpeed;
    AngleH += Input->CameraHorizontal * dt * CamSpeed * 20.0f;
    AngleV -= Input->CameraVertical * dt * CamSpeed * 20.0f;
    
    Camera->Eye.z = gb_cos(gb_to_radians(AngleH));
    Camera->Eye.x = gb_sin(gb_to_radians(AngleH));
    Camera->Eye.y = gb_sin(gb_to_radians(AngleV));
    
    
    // NOTE(Barret5Ocal): Gravity 
    v3 Gravity = {0.0f, 1.0f, 0.0f};
    //Velocity += Gravity;
    
    Player->Entity->Velocity = Velocity;
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
    
    if(MinX1 >= MaxX2) return 1;
    if(MinY1 >= MaxY2) return 1;
    if(MinZ1 >= MaxZ2) return 1;
    
    if(MinX2 >= MaxX1) return 1;
    if(MinY2 >= MaxY1) return 1;
    if(MinZ2 >= MaxZ1) return 1;
    
    return 0;
}

struct collision
{
    entity *Entity1;
    aabb AABB1; 
    
    entity *Entity2;
    aabb AABB2;
};

void TestCollision(entity *Entities, uint32 Count, memory_arena *Collisions)
{
    ResetArena(Collisions);
    
    for(uint32 Index = 0;
        Index < Count;
        ++Index)
    {
        entity *Entity1 = Entities + Index;
        //Entity1->Pos += Entity1->Velocity;
        
        for (uint32 Index2 = 0;
             Index2 < Count;
             ++Index2)
        {
            if(Index != Index2)
            {
                entity *Entity2 = Entities + Index2;
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
        }
    }
}

void ResolveCollision(entity *Entities, int32 EnititySize, collision *Collisions, int32 CollisionSize)
{
    if (CollisionSize > 0)
        int i = 0;
    
    // TODO(Barret5Ocal): finish this. i making 2 collision struct for each collision so make sure that you only modify one entity per collision
    for(uint32 Index = 0;
        Index < CollisionSize;
        ++Index)
    {
        collision *Collision = Collisions + Index;
        
        entity *Entity = Collision->Entity1;
        entity *Other = Collision->Entity2;
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
                
            }break;
            default:
            {
                InvalidCodePath;
            }break;
        }
    }
    
    for(uint32 Index = 0;
        Index < EnititySize;
        ++Index)
    {
        entity *Entity = Entities + Index;
        
        Entity->Pos += Entity->Velocity;
    }
}

void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    
    MovePlayer(GameState, Input, dt);
    
    TestCollision((entity *)GameState->Entities.Memory, GameState->Entities.Used/sizeof(entity), &GameState->Collisions);
    
    ResolveCollision((entity *)GameState->Entities.Memory, GameState->Entities.Used/sizeof(entity), (collision *)GameState->Collisions.Memory, GameState->Collisions.Used/sizeof(collision));
    
    render_setup *Setup = (render_setup *)PushStruct(RenderBuffer, render_setup);
    *Setup = {};
    Setup->CameraPos = GameState->Player.Entity->Pos;
    Setup->ViewDir = GameState->Player.Camera.Eye;
    
    
    for(int32 Index = 0;
        Index < GameState->Scene1->Count;
        ++Index)
    {
        level_block *Block = GameState->Scene1->Blocks + Index;
        
        render_element *Element = (render_element *)PushStruct(RenderBuffer, render_element);
        Element->Type = Block->Type;
        Element->Scale = Block->Entity->Scale;
        Element->Position = Block->Entity->Pos;
        Element->Angle = Block->Entity->Angle;
        Element->Axis = Block->Entity->Axis;
        Element->Material = 
        {
            {1.0f, 0.5f, 0.31f},
            {1.0f, 0.5f, 0.31f},
            {0.5f, 0.5f, 0.5f},
            32.0f
        };
        ++Setup->Count;
    }
    
}

