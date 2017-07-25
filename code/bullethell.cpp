void Setup(game_state *GameState)
{
    GameState->Camera.Pos = {0.0f, 0.0f, -10.0f};
    GameState->Camera.Eye = {0.0f, 0.0f, 1.0f};
}

void MoveCamera(game_state *GameState, input *Input, float dt)
{
    real32 Speed = 10.0f; 
    camera *Camera = &GameState->Camera; 
    if(Input->MoveVertical > 0) Camera->Pos += (Camera->Eye * dt * Speed);
    if(Input->MoveVertical < 0) Camera->Pos -= (Camera->Eye * dt * Speed);
    v3 Right; 
    gb_vec3_cross(&Right, Camera->Eye, {0.0f, 1.0f, 0.0f});
    if(Input->MoveHorizontal > 0) Camera->Pos += (Right * dt * Speed);
    if(Input->MoveHorizontal < 0) Camera->Pos -= (Right * dt * Speed);
    
#if 0
    gbMat4 Rot;
    gb_mat4_rotate(&Rot, {0.0f, 1.0f, 0.0f}, (3.14f/(4.0f*45.0f))* Input->MouseMove.x);
    gbMat3 Rot3;
    Rot3.x = Rot.x.xyz; 
    Rot3.y = Rot.y.xyz; 
    Rot3.z = Rot.z.xyz; 
    
    gb_mat3_mul_vec3(&Camera->Eye, &Rot3, Camera->Eye);
    char Buffer[100] = {}; 
    stbsp_sprintf(Buffer, "Eye: %f, %f, %f\n", Camera->Eye.x, Camera->Eye.y, Camera->Eye.z);
    
    OutputDebugStringA(Buffer);
#endif 
}

void Update(game_state *GameState, input *Input, float dt, memory_arena *RenderBuffer)
{
    v3 CubePositions[] = 
    {
        v3{ 0.0f,  0.0f,  0.0f},
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

