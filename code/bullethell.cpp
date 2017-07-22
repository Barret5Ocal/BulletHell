struct render_setup
{
    v3 CameraPos;
    v3 RotAxis;
    float Angle;
};

struct render_element
{
    int Type;
    v3 Position;
    v4 Quanterian;
    v3 Scale;
};

void Update(input *Input, memory_arena *RenderBuffer)
{
    render_setup *Setup = (render_setup *)PushStruct(RenderBuffer, render_setup);
    
    
}

