#define Kilobyte(Value) 1024 * Value
#define Megabyte(Value) 1024 * Kilobyte(Value)
#define Gigabyte(Value) 1024 * Megabyte(Value)
#define Terabyte(Value) 1024 * Gigabyte(Value)

struct memory_arena 
{
    char *Memory;
    int Size; 
    int Used; 
};

#define PushArray(Arena, Count, Type) PushSize(Arena, sizeof(Type) * Count)
#define PushStruct(Arena, Type) PushSize(Arena, sizeof(Type))

void *PushSize(memory_arena *Arena, size_t Size)
{
    void *Result = 0;
    if((Size + Arena->Used) < Arena->Size)
    {
        Result = Arena->Memory + Arena->Used; 
        Arena->Used += Size;
    }
    else 
        Assert(false);
    return Result; 
}

memory_arena PushArena(memory_arena *Arena, size_t Size)
{
    memory_arena Result = {};
    if((Size + Arena->Used) < Arena->Size)
    {
        Result.Memory = Arena->Memory + Arena->Used; 
        Result.Used = 0;
        Arena->Used += Size;
        Result.Size = Size; 
    }
    else 
    {
        Assert(false);
    }
    return Result; 
    
}

void ResetArena(memory_arena *Arena)
{
    Arena->Used = 0;
}

void AllocateArena(memory_arena *Arena, size_t Size)
{
    Arena->Size = Size; 
    Arena->Memory =(char *)VirtualAlloc(0, Arena->Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    Arena->Used = 0;
}