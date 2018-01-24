/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * CLI loop controller.
 *
 */

#ifndef CLION_CLI_LOOP_H
#define CLION_CLI_LOOP_H


#include "GameCore.h"

/**
 * Main loop for cli
 * @param game it is a pointer to array of 4 pointers
 * to instance of GameCore
 */
void cli_loop(GameCore ** game);

#endif //CLION_CLI_LOOP_H
