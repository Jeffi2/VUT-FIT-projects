/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing game core of single game
 *
 */

#ifndef CLION_GAMECORE_H
#define CLION_GAMECORE_H


#include "FactoryKlondike.hpp"
#include <fstream>
#include <algorithm>


class GameCore {
private:
    //! Factory for creating items
    FactoryKlondike* factory;
    //! Pack of cards
    CardDeck* standDeck;
    //! Stacker
    CardDeck* stacker;
    //! Working1 stack
    Working* working1;
    //! Working2 stack
    Working* working2;
    //! Working3 stack
    Working* working3;
    //! Working4 stack
    Working* working4;
    //! Working5 stack
    Working* working5;
    //! Working6 stack
    Working* working6;
    //! Working7 stack
    Working* working7;
    //! Clubs target stack
    Target* targetC;
    //! Hearts target stack
    Target* targetH;
    //! Spades target stack
    Target* targetS;
    //! Diamonds target stack
    Target* targetD;
    //! Object representing undo stack
    GlobalUndo* globalUndo;

public:
    /**
     * Create one game core
     */
    GameCore();
    /**
     * Save game state into file
     * @param path File path
     * @return
     */
    bool saveGame(std::string path);
    /**
     * Debug print
     */
    void printAll();
    /**
     * CLI print of the game
     */
    void print_cli();
    /**
     * Get move help
     * @return string with help
     */
    std::string getHelp();
    /**
     * Load game from file
     * @param path File path
     * @return true when load file is ok
     */
    bool loadGame(std::string path);
    /**
     * Return true when game is over.
     */
    bool winGame();
    /**
     * Exit game
     */
    ~GameCore();
    /**
     * Represents move between Pack an Stacker
     * @return true when move is ok
     */
    bool moveDtoS();
    /**
     * Represents move between Stacker an Working
     * @param w Pointer to working stack
     * @return true when move is ok
     */
    bool moveStoW(Working* w);
    /**
     * Represents move between Stacker an Target
     * @param t Pointer to target stack
     * @return true when move is ok
     */
    bool moveStoT(Target* t);
    /**
     * Move more cards between working stacks
     * @param src Source working stack
     * @param card Card to take from
     * @param dst Destination working stack
     * @return true when move is ok
     */
    bool moveMoreWtoW(Working* src, KCard* card, Working* dst);
    /**
     * Move one(top) card between working stacks
     * @param src Source working stack
     * @param dst Destination working stack
     * @return true when move is ok
     */
    bool moveOneWtoW(Working* src, Working* dst);
    /**
     * Represents move between Working an Target
     * @param src Working stack
     * @param t Target stack
     * @return true when move is ok
     */
    bool moveWtoT(Working* src, Target* t);
    /**
     * Represents move between Working an Target
     * @param w Working stack
     * @param src Target stack
     * @return true when move is ok
     */
    bool moveTtoW(Target* src, Working* w);
    /**
     * Make undo move
     */
    void undo();
    /**
     * Getter for Pack of card
     * @return pointer to Pack
     */
    CardDeck* getPack();
    /**
     * Getter for stacker
     * @return pointer to stacker
     */
    CardDeck* getStacker();
    /**
     * Getter for clubs target
     * @return pointer to clubs target
     */
    Target* getTargetC();
    /**
     * Getter for spades target
     * @return pointer to spades target
     */
    Target* getTargetS();
    /**
     * Getter for diamonds target
     * @return pointer to diamonds target
     */
    Target* getTargetD();
    /**
     * Getter for hearts target
     * @return pointer to hearts target
     */
    Target* getTargetH();
    /**
     * Getter for working1
     * @return Pointer to working1
     */
    Working* getWorking1();
    /**
     * Getter for working2
     * @return Pointer to working2
     */
    Working* getWorking2();
    /**
     * Getter for working3
     * @return Pointer to working3
     */
    Working* getWorking3();
    /**
     * Getter for working4
     * @return Pointer to working4
     */
    Working* getWorking4();
    /**
     * Getter for working5
     * @return Pointer to working5
     */
    Working* getWorking5();
    /**
     * Getter for working6
     * @return Pointer to working6
     */
    Working* getWorking6();
    /**
     * Getter for working7
     * @return Pointer to working7
     */
    Working* getWorking7();
    /**
     * Get maximum items of Working stack for
     * print_cli
     * @return  maximum items of Working stack
     */
    int max_Wstack();
};


#endif //CLION_GAMECORE_H
