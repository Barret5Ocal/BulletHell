
struct vertex
{
    v3 Pos;
    v3 Norm; 
};

struct model
{
    uint32 Count;
    vertex *Vertices;
    uint32 ICount;
    uint32 *Indices; 
};

struct render_setup
{
    uint32 Count; 
    v3 CameraPos;
    v3 ViewDir;
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
    quaternion Quaternion;
    v3 Scale;
    render_material Material;
    model  *Model;
    int32 Debug; 
};

//typedef gbAabb3 aabb;

struct aabb
{
    gbVec3 centre, half_size;
    model *Shape;
};

enum entity_type
{
    LEVEL_BLOCK,
    BULLET,
    PLAYER
};

struct entity 
{
    uint32 ID;
    uint32 Type;
    v3 Pos;
    quaternion Quaternion;
    v3 Scale;
    v3 Velocity;
    aabb Aabb;
    model *Model;
};

struct velocity 
{
    entity *Entity;
    v3 Velocity; 
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
    real32 Speed;
};

struct bullet
{
    uint32 ID;
    entity *Entity;
    v3 Direction;
    real32 Speed; 
    uint32 AliveTime; 
};


struct game_state
{
    player Player;
    
    memory_arena SceneArena;
    scene_layout *Scene1; 
    memory_arena Models;
    memory_arena Collisions;
    
    uint32 IDCountB = 1; 
    dynamic_arena Bullets; 
    //memory_arena Entities;
    
    uint32 IDCount = 1; 
    dynamic_arena Entities;
    memory_arena Velocities; 
};
