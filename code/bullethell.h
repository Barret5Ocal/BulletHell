
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
    uint32 Type;
    v3 Position;
    v3 Axis;
    float Angle;
    v3 Scale;
    render_material Material;
};

typedef gbAabb3 aabb;

struct vertex
{
    v3 Pos;
    v3 Norm; 
};

struct model
{
    uint32 Count;
    vertex *Vertices;
};

struct entity 
{
    v3 Pos;
    real32 Angle;
    v3 Axis;
    v3 Scale;
    v3 Velocity;
    aabb Aabb;
    model *Model;
};

struct level_block
{
    uint32 Type; 
    entity *Entity;
};

struct scene_layout
{
    uint32 Count;
    level_block *Blocks;
};

struct camera 
{
    //v3 Pos;
    v3 Eye; 
};

struct player 
{
    camera Camera;
    entity *Entity;
};

struct bullet
{
    entity *Entity;
};

struct game_state
{
    player Player;
    
    memory_arena SceneArena;
    scene_layout *Scene1; 
    memory_arena Entities;
    memory_arena Models;
};
