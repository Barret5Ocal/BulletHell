
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

struct level_block
{
    uint32 Type; 
    v3 Pos;
    real32 Angle;
    v3 Axis;
    v3 Scale;
};

struct scene_layout
{
    uint32 Count;
    level_block *Blocks;
};

struct camera 
{
    v3 Pos;
    v3 Eye; 
};

struct player 
{
    
};

struct bullet
{
    
};

struct scene
{
    // NOTE(Barret5Ocal): holds references to all objects in the game 
    scene_layout *Layout;
    player *Players;
    int32 PlayerCount;
    bullet *Bullets;
    int32 BulletCount;
};

struct game_state
{
    camera Camera; 
    memory_arena SceneArena;
    scene Scene; 
    scene_layout *Scene1; 
    
};
