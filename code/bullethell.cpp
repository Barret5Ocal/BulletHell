void Setup(game_state *GameState)
{
    GameState->CameraPos = {0.0f, 0.0f, -10.0f};
    GameState->ViewDir = {0.0f, 0.0f, 0.0f};// NOTE(Barret5Ocal): this should stay as a unit sphere
    GameState->ViewDegree = 270.0f;
    //GameState->ViewDegree = 90.0f;
    
}

void MoveCamera(game_state *GameState, input *Input, float dt)
{
    real32 Speed = 5.0f;
    v3 Movement  = {(real32)Input->MoveHorizontal * dt * Speed, 0.0f, (real32)Input->MoveVertical * dt * Speed};
    GameState->CameraPos += Movement;
    
    v3 CameraMov  = {(real32)Input->CameraHorizontal * dt * Speed, 0.0f, (real32)Input->CameraVertical * dt * Speed};
    GameState->ViewDir += CameraMov;
#if 0
    GameState->ViewDegree += Input->CameraHorizontal; 
    if(GameState->ViewDegree > 360.0f) GameState->ViewDegree = 0;
    if(GameState->ViewDegree < 0.0f) GameState->ViewDegree = 360;
    float rad = gb_to_radians(GameState->ViewDegree);
    float sin = gb_sin(rad);
    float cos = gb_cos(rad);
    v3 CameraRot = {cos, 0.0f, sin};
    
    GameState->ViewDir = CameraRot;
#endif 
}

void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    v3 CubePositions[] = 
    {
        -v3{ 0.0f,  0.0f,  0.0f},
        -v3{2.0f,  5.0f, -15.0f},
        -v3{-1.5f, -2.2f, -2.5f},
        -v3{-3.8f, -2.0f, -12.3f},
        -v3{2.4f, -0.4f, -3.5f},
        -v3{-1.7f,  3.0f, -7.5f},
        -v3{1.3f, -2.0f, -2.5f},
        -v3{1.5f,  2.0f, -2.5f},
        -v3{1.5f,  0.2f, -1.5f},
        -v3{-1.3f,  1.0f, -1.5f}
    };
    
    MoveCamera(GameState, Input, dt);
    
    render_setup *Setup = (render_setup *)PushStruct(RenderBuffer, render_setup);
    Setup->CameraPos = GameState->CameraPos;
    Setup->ViewDir = GameState->ViewDir;
    
    for(int32 Index = 0;
        Index < ArrayCount(CubePositions);
        ++Index)
    {
        
        render_element *Element = (render_element *)PushStruct(RenderBuffer, render_element);
        Element->Type = BOX;
        Element->Scale = {1.0f, 1.0f, 1.0f};
        Element->Position = {CubePositions[Index].x, CubePositions[Index].y, CubePositions[Index].z};
        Element->Angle = 20.0f * Index;
        Element->Axis = {1.0f, 0.3f, 0.5f};
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

