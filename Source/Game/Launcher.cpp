#include "Game/Bootstrap.h"

#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Game/SampleGame.h"

int main(int argc, char* argv[])
{
    SampleGame* game = hdNew(hd::mem::Persistent(), SampleGame)();

    game->Run();

    hdSafeDelete(hd::mem::Persistent(), game);

    return 0;
}