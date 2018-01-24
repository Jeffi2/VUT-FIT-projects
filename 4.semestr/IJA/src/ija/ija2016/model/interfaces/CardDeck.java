/**
 * IJA - project
 * @file: CardDeck.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.interfaces;

import java.util.ArrayList;

/**
 * Class represents interface of card deck
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public interface CardDeck extends java.io.Serializable{
    /**
     *
     * @return card from actual position without removing
     */
    Card get();

    /**
     * get card from index without removing
     * @param index index to array list
     * @return card
     */
    Card get(int index);

    /**
     * check whether card deck is empty
     * @return true if empty
     */
    boolean isEmpty();

    /**
     * give actual card and remove it from card stack
     * @return card
     */
    Card pop();

    /**
     * inster card into card deck
     * @param card card
     * @param invokeUP true if super method should be call
     * @return true if success
     */
    boolean put(Card card, boolean invokeUP);

    /**
     * check size of card deck
     * @return nubmer of items in card deck
     */
    int size();

    /**
     * print all items into terminal
     * used for debugging
     */
    void print();

    /**
     * suffle cards in card deck
     * use after create
     */
    void suffle();

    /**
     * move one card from card deck and add to another (working )
     * @param source usually card deck
     * @param dest usually one of working
     */
    void moveOneCard(CardDeck source, CardDeck dest);
    void undo();

    /**
     * save card stack from object into file, first object , which call save create file descriptor to file , others only use it
     * @param file name of output file
     * @return true if success
     */
    boolean save(String file);

    /**
     * iterate over all items from list and call method next, as argument send all cards which can be moved(card by card)
     * @param list list of all items(stacks ) in game
     * @return object ( stack) where we can add testing card
     */
    CardDeck help(ArrayList list);

    /**
     * method is used after load
     * method change card stack inside object
     * @param new_list loaded list with cards
     */
    void changelist(ArrayList<Card> new_list);

    /**
     *return itself name generated during construction
     * @return name of itself
     */
    String name();

}
