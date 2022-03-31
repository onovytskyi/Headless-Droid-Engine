#include "Game/Bootstrap.h"

#include "Engine/Memory/EngineMemoryInterface.h"
#include "Game/SampleGame.h"

int main(int argc, char* argv[])
{
    SampleGame* game = hdNew(hd::Persistent(), SampleGame)();

    game->Run();

    hdSafeDelete(hd::Persistent(), game);

    return 0;
}