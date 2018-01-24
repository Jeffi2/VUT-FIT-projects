/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing stack of cards.
 *
 */

#ifndef CLION_CARDSTACK_H
#define CLION_CARDSTACK_H

#include "CardDeck.hpp"
#include "card.hpp"
class CardStack : public CardDeck
{
private:
    /**
     * Delete first card from CardList
     */
    void remove();
    /**
     * Add Card to CardList
     * @param card Card
     */
    void add(KCard* card);
public:
    CardStack(){
    }
    CardStack(GlobalUndo* undo) {
    }
    /**
     * Add stack of cards into CardList and controll rules
     * @param stack Stack with cards to put
     * @return true when put is possible
     */
    virtual bool put(CardStack* stack);
    /**
     * Uses base put
     * @param stack Stack of cards to put
     * @return true when put success
     */
    virtual bool superput(CardStack* stack);
    /**
     * Get stack of cards from some card
     * @param card Card to take from
     * @return stack with cards to move
     */
    CardStack* pop(KCard* card);
    /**
     * Move stacks of cards between others
     * @param source source Stack
     * @param card Card to move from
     * @param dest destination Stack
     * @return true when move is ok
     */
    bool moveCards(CardStack* source, KCard* card, CardStack* dest);
    class CardStackCMD:public Command {
    private:
        //! Invoker
        CardStack* stack;
        //! Source stack
        CardStack* source;
        //! Card for take from
        KCard* card;
        //! Destination stack
        CardStack* dest;
        //! Flipped after move?
        bool turnedFaceUp;
    public:
        /**
         * Constructor of CardStack move, move stack of cards between decks
         * @param obj Invoker
         * @param card From that card take and move
         * @param source Source Stack
         * @param dest Target Stack
         */
        CardStackCMD(CardStack* obj, CardStack* source, KCard* card, CardStack* dest);
        virtual bool execute() override;
        virtual void undo() override;
    };
};


#endif //CLION_CARDSTACK_H
