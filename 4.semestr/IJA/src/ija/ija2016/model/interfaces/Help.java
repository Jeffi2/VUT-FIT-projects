/**
 * IJA - project
 * @file: Help.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.interfaces;

import java.util.ArrayList;

/**
 * Class represents interface for hepl
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public interface Help {
    /**
     * same method like push, but without adding items only test
     * @param c tested card
     * @return true if card c can be add to dest object
     */
    boolean next(Card c);

    /**
     *
     * @param index index of required card
     * @return  required card
     */
    Card get(int index);

    /**
     * check if destinaniton obejct is empty or not
     * @return true if empty
     */
    boolean isEmpty();

    /**
     *
     * @param list list of all stacks in game
     * @return return stack where we can add tested card
     */
    CardDeck help(ArrayList list);
}
