/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing working stack for cards
 *
 */

#ifndef CLION_WORKING_H
#define CLION_WORKING_H


#include "CardStack.h"

class Working : public CardStack{
public:
    /**
     * Constructor for working stack
     * @param undo pointer to undo object
     * @param count number of cards to put
     */
    Working(GlobalUndo* undo, int count);
    /**
     * Puts card into CardList and controle rules for put
     * @param card Card to put
     * @return true when put success
     */
    bool put(KCard* card);
    /**
     * Base Card put
     * @param card Card
     * @return true when put success
     */
    bool superput(KCard* card);
    /**
     * Put stack of cards to working stack and control rules
     * @param stack Stack with cards to put
     * @return true when put success
     */
    bool put(CardStack* stack) override ;
    /**
     * Use base put
     * @param stack Stack with cards to put
     * @return true when put success
     */
    bool superput(CardStack* stack);
    //should move card be virtual?
    /**
     * Move card to working stack
     * @param source Source deck
     * @param dest Destination deck
     * @return true when move succes
     */
    bool moveCard(CardDeck* source ,CardDeck* dest) override;
    /**
     * Simulate if put to working stack is possible
     * @param card Card to put
     * @return true when put is possible
     */
    virtual bool next(KCard* card) override ;
    /**
     * Is there any stack that can put card?
     * @param list of all stacks
     * @return stack pointer where card can be put
     */
    virtual CardDeck* help(std::vector<CardDeck*> *list) override;

    class WorkingCMD:public Command {
    private:
        //! Invoker
        Working* stack;
        //! Source deck
        CardDeck* source;
        //! Destination stack
        CardDeck* dest;
        //! Flipped card after move?
        bool turnedDown;
    public:
        /**
         * Constructor of move card to working stack
         * @param obj Invoker
         * @param src Source Deck
         * @param dest Target Deck
         */
        WorkingCMD(Working* obj, CardDeck* src, CardDeck* dest );
        virtual bool execute() override;
        virtual void undo() override;
    };
};


#endif //CLION_WORKING_H
