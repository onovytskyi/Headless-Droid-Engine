#include "Game/Bootstrap.h"

#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Game/SampleGame.h"

int main(int argc, char* argv[])
{
    hd::mem::AllocationScope persistentScope(hd::mem::GetPersistentAllocator());

    SampleGame* game = persistentScope.AllocateObject<SampleGame>();

    game->Run();

    return 0;
}