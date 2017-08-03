void GenerateAABB(entity *Entity)
{
    aabb *AABB = &Entity->Aabb;
    
    // TODO(Barret5Ocal): generate aabb
    uint32 Count = Entity->Model->Count;
    m4 Rotate;
    gb_mat4_rotate(&Rotate, Entity->Axis, gb_to_radians(Entity->Angle));
    m4 Scale;
    gb_mat4_scale(&Scale, Entity->Scale);
    m4 Trans;
    gb_mat4_translate(&Trans, Entity->Pos);
    
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
        Pos4 =  Trans * Scale * Rotate * Pos4;
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
    
    // TODO(Barret5Ocal): Make AABB now
}

scene_layout *LoadSceneLayout(game_state *GameState)
{
    AllocateArena(&GameState->SceneArena, Megabyte(2));
    GameState->Scene1 =(scene_layout *)PushStruct(&GameState->SceneArena, scene_layout);
    scene_layout *Scene = GameState->Scene1;
    
    Scene->Count = 5;
    Scene->Blocks = (level_block *)(Scene + 1);
    level_block *Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    entity *Entity = (entity *)PushStruct(&GameState->Entities, entity);
    Block->Entity = Entity; 
    Block->Type = BOX; 
    Entity->Pos = {0.0f, -3.0f, 0.0f};
    Entity->Angle = 0;
    Entity->Axis = {1.0f, 1.0f, 1.0f};
    Entity->Scale = {100.0f, 1.0f, 100.0f};
    Entity->Model = (model *)GameState->Models.Memory;
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Entity = (entity *)PushStruct(&GameState->Entities, entity);
    Block->Entity = Entity; 
    Block->Type = BOX; 
    Entity->Pos = {47.5f, 2.0f, 47.5f};
    Entity->Angle = 0;
    Entity->Axis = {1.0f, 1.0f, 1.0f};
    Entity->Scale = {5.0f, 10.0f, 5.0f};
    Entity->Model = (model *)GameState->Models.Memory;
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Entity = (entity *)PushStruct(&GameState->Entities, entity);
    Block->Entity = Entity; 
    Block->Type = BOX; 
    Entity->Pos = {-47.5f, 2.0f, 47.5f};
    Entity->Angle = 0;
    Entity->Axis = {1.0f, 1.0f, 1.0f};
    Entity->Scale = {5.0f, 10.0f, 5.0f};
    Entity->Model = (model *)GameState->Models.Memory;
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Entity = (entity *)PushStruct(&GameState->Entities, entity);
    Block->Entity = Entity; 
    Block->Type = BOX; 
    Entity->Pos = {47.5f, 2.0f, -47.5f};
    Entity->Angle = 0;
    Entity->Axis = {1.0f, 1.0f, 1.0f};
    Entity->Scale = {5.0f, 10.0f, 5.0f};
    Entity->Model = (model *)GameState->Models.Memory;
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Entity = (entity *)PushStruct(&GameState->Entities, entity);
    Block->Entity = Entity; 
    Block->Type = BOX; 
    Entity->Pos = {-47.5f, 2.0f, -47.5f};
    Entity->Angle = 0;
    Entity->Axis = {1.0f, 1.0f, 1.0f};
    Entity->Scale = {5.0f, 10.0f, 5.0f};
    Entity->Model = (model *)GameState->Models.Memory;
    
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
    
    LoadSceneLayout(GameState);
}

void TestCollision(entity *Entities)
{
    
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
    
    Player->Entity->Pos += Velocity;
}


void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    
    MovePlayer(GameState, Input, dt);
    
    //TestCollision(&GameState->Scene);
    
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

