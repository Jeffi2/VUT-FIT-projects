/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing target stack for cards
 *
 */

#ifndef CLION_TARGET_H
#define CLION_TARGET_H


#include "CardDeck.hpp"

class Target : public CardDeck{
private:
    CardColor::Color packColor;
public:
    /**
     * Constructor for target stack
     * @param color Color that stack accept
     * @param undo Pointer to undo stack object
     * @param stack Name for stack
     */
    Target(CardColor::Color color, GlobalUndo* undo, std::string stack);
    /**
     * Put cart to target stack and control rules
     * @param card Card to put
     * @return true when put success
     */
    bool put(KCard* card);
    //should move card be virtual?
    /**
     * Move one card to target stack
     * @param source Deck
     * @param dest Target deck
     * @return true when move is ok
     */
    bool moveCard(CardDeck* source, CardDeck* dest) override;
    /**
     * Simulate put to CardList with rules
     * @param card Card to put
     * @return true is put is possible
     */
    virtual bool next(KCard* card) override;

    class TargetStackCMD:public Command{
    private:
        //! Ivoker
        Target* stack;
        //! Source deck
        CardDeck* source;
        //! Destination deck
        CardDeck* dest;
        //! Flipped card after move?
        bool turnedDown;
    public:
        /**
         * Constructor of Target move, move card to target
         * @param obj Invoker
         * @param src Source Deck
         * @param dest Target Deck
         */
        TargetStackCMD(Target* obj, CardDeck* src, CardDeck* dest);
        virtual bool execute() override;
        virtual void undo() override;
    };
};


#endif //CLION_TARGET_H
