/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing pack of cards.
 *
 */

#ifndef CardDeck_hpp
#define CardDeck_hpp

#include "card.hpp"
#include <stdio.h>
#include <vector>
#include <iostream>

class Command{
public:
    /**
     * Method for executing moves
     * @return true when move success
     */
    virtual bool execute(){} // return false?
    /**
     * Make undo move
     */
    virtual void undo(){}
};

class Invoker{
private:
    //! List of all moves
    std::vector<Command*> list;
public:
    /**
     * Executing move
     * @param cmd move command
     * @return true when move success
     */
    bool execute(Command* cmd);
    /**
     * Make undo move
     */
    void undo();
    //! Delete list of moves
    ~Invoker();
};

class GlobalUndo
{
private:
    //! List of all invokers
    std::vector<Invoker*> undoStack;
public:
    /**
     * Add invoker to list
     * @param invoker Stack that do moves
     */
    void put(Invoker* invoker);
    /**
     * Make undo of last game move
     */
    void makeUndo();
    /**
     * Delete all pointers in undoStack
     */
    void removePointers();
};

class CardDeck
{
protected:
    //! List of cards
    std::vector<KCard*> CardList;
    //! Invoker object
    Invoker* invoker;
    //! Pointer to global undo object (each for one game)
    GlobalUndo* globalUndo;
    //! Name of stack
    std::string nam;
public:
    /**
     * Simulate put in stack, used for help
     * @param card Card
     * @return true when is possible to add card
     */
    virtual bool next(KCard* card);
    /**
     * Delete all pointer in CardList
     */
    void deletePonters();
    /**
     * Constructor of standard deck
     */
    CardDeck();
    /**
     * Constuructor that save pointer to undo stack
     */
    CardDeck(GlobalUndo* undo);
    /**
     * Shuffle card pack
     */
    void suffle();
    /**
     * Get top card
     * @return top Card pointer and delete from CardList
     */
    KCard *pop();
    /**
     * Get top card pointer
     * @return top Card pointer
     */
    KCard *get();
    /**
     * Get pointer to card on index
     * @param index to CardList
     * @return pointer to card on index in reverted way
     */
    KCard *get(int index);
    /**
     * Get pointer to card on index
     * @param index to CardList
     * @return pointer to card on index in normal way
     */
    KCard *getRealIn(int index);
    /**
     * Is CardList empty?
     * @return true when CardList empty
     */
    bool isEmpty();
    /**
     * Base Card put
     * @param card Card
     * @return true when put success
     */
    virtual bool superput(KCard* card);
    /**
     * Is there any stack that can put card?
     * @param list of all stacks
     * @return stack pointer where card can be put
     */
    virtual CardDeck* help(std::vector<CardDeck*> *list);
    /**
     * Getter for deck name
     * @return deck name
     */
    std::string name();
    /**
     * Puts card into CardList and controle rules for put
     * @param card Card to put
     * @return true when put success
     */
    virtual bool put(KCard* card);
    /**
     * Add Card to end of CardList used in load
     * @param card Card
     * @return true when put success
     */
    bool putToEnd(KCard* card);
    /**
     * Getter for size
     * @return CardList size
     */
    int size();
    /**
     * Debug function for CardDeck print
     */
    void print();

    std::string str_card(int index);
    std::string str_LastCard();
    /**
     * Getter for invore
     * @return pointer to invoker
     */
    Invoker* getInvoker();
    /**
     * Getter for undo stack
     * @return pointer to undo stack
     */
    GlobalUndo* getUndoStack();
    /**
     * Move card from deck to deck
     * @param source Deck
     * @param dest Deck
     * @return true when move is possible
     */
    virtual bool moveCard(CardDeck* source ,CardDeck* dest);
    /**
     * Saves CardList into file
     * @param file File stream
     * @return true when save is ok
     */
    bool save(std::ofstream & file);
    /**
     * Load CardList from file
     * @param file File stream
     * @return true when load is ok
     */
    bool load(std::ifstream & file);

    //This command is only for move from card pack to stacker!!!
    class CardDeckCMD:public Command {
    private:
        //! Ponter to invoker
        CardDeck* stack;
        //! Source stack
        CardDeck* source;
        //! Destination stack
        CardDeck* dest;
    public:
        /**
         * Constructor of CardDeck move, move cards between decks
         * @param obj Invoker
         * @param source Source Deck
         * @param dest Target Deck
         */
        CardDeckCMD(CardDeck* obj, CardDeck* source, CardDeck* dest);
        virtual bool execute() override;
        virtual void undo() override ;
    };
    //! Delete all dynamic items
    ~CardDeck();
};

#endif /* CardDeck_hpp */

