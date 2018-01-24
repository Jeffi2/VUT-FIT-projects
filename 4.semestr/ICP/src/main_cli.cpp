/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Control program flow, starts application
 *
 */

#include <stdlib.h>
#include "GameCore.h"
#include "cli_loop.h"

int main(int argc, const char * argv[])
{
    GameCore * game[4] = {nullptr,};
    game[0] = new GameCore();
    cli_loop(game);


    for (int i = 0; i < 4; ++i)
    {
        if(game[i] != nullptr)
            delete(game[i]);
    }

    return 0;
}
