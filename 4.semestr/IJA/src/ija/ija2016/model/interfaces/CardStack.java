/**
 * IJA - project
 * @file: CardStack.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.interfaces;

/**
 * Class represents interface of card stack
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public interface CardStack extends CardDeck {
    /**
     * Take stack of cards from specific card.
     * @param card take from this card
     * @return Stack of cards.
     */
    CardStack pop(Card card);

    /**
     * Inserts card stack into stack end.
     * @param stack moving stack of cards
     * @param trash sets super method call (true for super call)
     * @return true when put success
     */
    boolean put(CardStack stack, boolean trash);

    /**
     * move cards all card before card from source stack to destination stack
     * @param source source card stack
     * @param card  card used like delimiter
     * @param dest destination stack
     */
    void moveMoreCards(CardStack source,Card card, CardStack dest);
}
