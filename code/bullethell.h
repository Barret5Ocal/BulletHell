struct render_setup
{
    uint32 Count; 
    v3 CameraPos;
    v3 ViewDir;
};

enum render_type
{
    BOX,
};

struct render_material
{
    v3 Ambient;
    v3 Diffuse;
    v3 Specular;
    float Shininess;
};

struct render_element
{
    int Type;
    v3 Position;
    v3 Axis;
    float Angle;
    v3 Scale;
    render_material Material;
};

struct game_state
{
    v3 CameraPos;
    v3 ViewDir; 
};
