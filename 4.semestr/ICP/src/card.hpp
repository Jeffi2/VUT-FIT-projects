/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing card.
 *
 */

#ifndef card_hpp
#define card_hpp

#include <stdio.h>
/**
 * Enum representing card color
 */
namespace CardColor {
    enum Color
    {
        clubs, spides, hearts, diamonds,
    };
}

class KCard
{
private:
    //! Cards color
    CardColor::Color col;
    //! Cards value
    int val;
    //! Is face up?
    bool face = false;
    
public:
    /**
     * Constructor for standard card
     * @param color Cards color
     * @param value Cards value
     */
    KCard(CardColor::Color color, int value);
    /**
     * Constoructor for loading cards from file
     * @param color Cards color
     * @param value Cards value
     * @param turn Card turned up
     */
    KCard(CardColor::Color color, int value, bool turn);
    /**
     * Color getter
     * @return Cards color
     */
    CardColor::Color color();
    /**
     * Compare two cards value
     * @param c Card to compare
     * @return int differenc between cards
     */
    int compareValue(KCard* c);
    /**
     * Compare two cards
     * @param c color
     * @return true if cards have same color and value
     */
    bool same_card(KCard *c);
    /**
     * Tests if card is turned up
     * @return true when face is true
     */
    bool isTurnedFaceUp();
    /**
     * Tests if two cards have same color(red/black)
     * @param c card
     * @return true when color is similar
     */
    bool similarColorTo(KCard* c);
    /**
     * Turns face up;
     * @return true when flipped
     */
    bool turnFaceUp();
    /**
     * Turns face down
     * @return true when flipped
     */
    bool turnFaceDown();
    /**
     * Getter for value
     * @return cards value
     */
    int value();
};


#endif /* card_hpp */
