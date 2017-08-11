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

// TODO(Barret5Ocal): We need a better allocator. One that will allow us to add, remove, and reorganize the data at will 

struct dynamic_arena
{
    uint32 TypeSize;
    uint32 AmountStored;
    uint32 *MemoryFills;
    uint8 *Memory;
    uint32 Size; 
};

void AllocateDynamic(dynamic_arena *Arena, size_t Size, size_t TypeSize)
{
    Arena->Size = Size; 
    Arena->TypeSize = TypeSize;
    Arena->AmountStored = 0;
    Arena->Memory =(uint8 *)VirtualAlloc(0, Arena->Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    Arena->MemoryFills =(uint32 *)VirtualAlloc(0, Arena->Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    for(uint32 *Index = Arena->MemoryFills;
        Index < Arena->MemoryFills + (Arena->Size / sizeof(uint32));
        ++Index)
    {
        *Index = 0;
    }
    
}

void *PushSize(dynamic_arena *Arena)
{
    void *Result  = 0;
    for(uint32 Index = 0;
        Index < (Arena->Size / sizeof(uint32));
        ++Index)
    {
        if(!*(Arena->MemoryFills + Index))
        {
            uint32 *UIndex = Arena->MemoryFills + Index;
            *UIndex = 1; 
            Result = Arena->Memory + (Index * Arena->TypeSize);
            ++Arena->AmountStored; 
            break; 
        }
    }
    
    return Result; 
}

void RemoveSize(dynamic_arena *Arena, void *MemoryElement)
{
    if(MemoryElement && Arena->AmountStored)
    {
        for(uint32 Index = 0;
            Index < Arena->Size / Arena->TypeSize;
            ++Index)
        {
            uint8 *Element = Arena->Memory + (Index * Arena->TypeSize);
            if (Element == MemoryElement)
            {
                uint32 *MemIndex = Arena->MemoryFills + Index;
                *MemIndex = 0;
                --Arena->AmountStored;
            }
        }
        
    }
}

