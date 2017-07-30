void Setup(game_state *GameState)
{
    GameState->Camera.Pos = {0.0f, 0.0f, -10.0f};
    //GameState->Camera.Eye = {0.559f, 0.0f, 0.829f};
    GameState->Camera.Eye = {0.0f, 0.0f, 1.0f};
    
}

void MoveCamera(game_state *GameState, input *Input, float dt)
{
    real32 Speed = 10.0f; 
    camera *Camera = &GameState->Camera; 
    if(Input->MoveVertical > 0) Camera->Pos += (Camera->Eye * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    if(Input->MoveVertical < 0) Camera->Pos -= (Camera->Eye * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    v3 Right; 
    gb_vec3_cross(&Right, Camera->Eye, {0.0f, 1.0f, 0.0f});
    if(Input->MoveHorizontal > 0) Camera->Pos -= (Right  * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    if(Input->MoveHorizontal < 0) Camera->Pos += (Right  * v3{1.0f, 0.0f, 1.0f} * dt * Speed);
    
#if 1
    real32 CamSpeed = 10.0f;
    static real32 AngleH = 0.0f;
    static real32 AngleV = 0.0f;
    AngleH += Input->MouseMove.x * dt * CamSpeed;
    AngleV += Input->MouseMove.y * dt * CamSpeed;
    Camera->Eye.z = gb_cos(gb_to_radians(AngleH));
    Camera->Eye.x = gb_sin(gb_to_radians(AngleH));
    Camera->Eye.y = gb_sin(gb_to_radians(AngleV));
#endif 
}

void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    v3 CubePositions[] = 
    {
        v3{0.0f,  0.0f,  0.0f},
        //v3{2.0f,  5.0f, -15.0f},
        //v3{-1.5f, -2.2f, -2.5f},
        //v3{-3.8f, -2.0f, -12.3f},
        //v3{2.4f, -0.4f, -3.5f},
        //v3{-1.7f,  3.0f, -7.5f},
        //v3{1.3f, -2.0f, -2.5f},
        //v3{1.5f,  2.0f, -2.5f},
        //v3{1.5f,  0.2f, -1.5f},
        //v3{-1.3f,  1.0f, -1.5f}
    };
    
    MoveCamera(GameState, Input, dt);
    
    render_setup *Setup = (render_setup *)PushStruct(RenderBuffer, render_setup);
    Setup->CameraPos = GameState->Camera.Pos;
    Setup->ViewDir = GameState->Camera.Eye;
    
    for(int32 Index = 0;
        Index < ArrayCount(CubePositions);
        ++Index)
    {
        
        render_element *Element = (render_element *)PushStruct(RenderBuffer, render_element);
        Element->Type = BOX;
        Element->Scale = {1.0f, 1.0f, 1.0f};
        Element->Position = {CubePositions[Index].x, CubePositions[Index].y, CubePositions[Index].z};
        Element->Angle = 0; //20.0f * Index;
        Element->Axis = {1.0f, 1.0f, 1.0f}; //{1.0f, 0.3f, 0.5f};
        Element->Material = 
        {
            {1.0f, 0.5f, 0.31f},
            {1.0f, 0.5f, 0.31f},
            {0.5f, 0.5f, 0.5f},
            32.0f
        };
        ++Setup->Count;
    }
    
    render_element *Element = (render_element *)PushStruct(RenderBuffer, render_element);
    Element->Type = BOX;
    Element->Scale = {10.0f, 1.0f, 10.0f};
    Element->Position = {0.0f, -5.0f, 0.0f};
    Element->Angle = 0;
    Element->Axis = {1.0f, 1.0f, 1.0f};
    Element->Material = 
    {
        {1.0f, 0.5f, 0.31f},
        {1.0f, 0.5f, 0.31f},
        {0.5f, 0.5f, 0.5f},
        32.0f
    };
    ++Setup->Count;
}

