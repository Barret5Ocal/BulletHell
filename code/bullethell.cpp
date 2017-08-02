scene_layout *LoadSceneLayout(game_state *GameState)
{
    AllocateArena(&GameState->SceneArena, Megabyte(2));
    GameState->Scene1 =(scene_layout *)PushStruct(&GameState->SceneArena, scene_layout);
    scene_layout *Scene = GameState->Scene1;
    
    
    Scene->Count = 5;
    Scene->Blocks = (level_block *)(Scene + 1);
    level_block *Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Block->Type = BOX; 
    Block->Pos = {0.0f, -3.0f, 0.0f};
    Block->Angle = 0;
    Block->Axis = {1.0f, 1.0f, 1.0f};
    Block->Scale = {100.0f, 1.0f, 100.0f};
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Block->Type = BOX; 
    Block->Pos = {47.5f, 2.0f, 47.5f};
    Block->Angle = 0;
    Block->Axis = {1.0f, 1.0f, 1.0f};
    Block->Scale = {5.0f, 10.0f, 5.0f};
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Block->Type = BOX; 
    Block->Pos = {-47.5f, 2.0f, 47.5f};
    Block->Angle = 0;
    Block->Axis = {1.0f, 1.0f, 1.0f};
    Block->Scale = {5.0f, 10.0f, 5.0f};
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Block->Type = BOX; 
    Block->Pos = {47.5f, 2.0f, -47.5f};
    Block->Angle = 0;
    Block->Axis = {1.0f, 1.0f, 1.0f};
    Block->Scale = {5.0f, 10.0f, 5.0f};
    
    Block = (level_block *)PushStruct(&GameState->SceneArena, level_block);
    Block->Type = BOX; 
    Block->Pos = {-47.5f, 2.0f, -47.5f};
    Block->Angle = 0;
    Block->Axis = {1.0f, 1.0f, 1.0f};
    Block->Scale = {5.0f, 10.0f, 5.0f};
    
    return Scene; 
}

void Setup(game_state *GameState)
{
    GameState->Camera.Pos = {0.0f, 0.0f, -10.0f};
    //GameState->Camera.Eye = {0.559f, 0.0f, 0.829f};
    GameState->Camera.Eye = {0.0f, 0.0f, 1.0f};
    
    GameState->Scene.Layout = LoadSceneLayout(GameState);
}

void TestCollision(scene *Scene)
{
    
}

void MoveCamera(game_state *GameState, input *Input, float dt)
{
    v3 Velocity = {}; 
    
    real32 Speed = 10.0f; 
    camera *Camera = &GameState->Camera; 
    
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
    
    Camera->Pos += Velocity;
}


void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    
    MoveCamera(GameState, Input, dt);
    
    TestCollision(&GameState->Scene);
    
    render_setup *Setup = (render_setup *)PushStruct(RenderBuffer, render_setup);
    *Setup = {};
    Setup->CameraPos = GameState->Camera.Pos;
    Setup->ViewDir = GameState->Camera.Eye;
    
    
    for(int32 Index = 0;
        Index < GameState->Scene1->Count;
        ++Index)
    {
        level_block *Block = GameState->Scene1->Blocks + Index;
        
        render_element *Element = (render_element *)PushStruct(RenderBuffer, render_element);
        Element->Type = Block->Type;
        Element->Scale = Block->Scale;
        Element->Position = Block->Pos;
        Element->Angle = Block->Angle;
        Element->Axis = Block->Axis;
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

