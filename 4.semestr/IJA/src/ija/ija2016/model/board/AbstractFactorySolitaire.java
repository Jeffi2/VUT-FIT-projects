/**
 * IJA - project
 * @file: AbstractFactorySolitaire.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */

package ija2016.model.board;

import ija2016.model.classes.GlobalUndo;
import ija2016.model.interfaces.*;

import java.util.ArrayList;
/**
 * Abstract class for factory pattern
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public abstract class AbstractFactorySolitaire {
    /**
     * add object to object list used for help
     * @param obj saved object
     */
    public abstract void add(Object obj);

    /**
     *
     * @return list with added item with method add
     */
    public abstract ArrayList<Object> list();

    /**
     * create and return Card
     * @param color of new card
     * @param value of new card
     * @return created card
     */
    public abstract Card createCard(Card.Color color, int value);

    /**
     * create Card deck with 52 cards
     * @param stack for undo operation
     * @return created Card deck
     */
    public abstract CardDeck createCardDeck(GlobalUndo stack);

    /**
     * Create target pack , add to list for help using method add
     * @param color of target pack
     * @param stack for undo operation
     * @return created Target pack
     */
    public abstract CardDeck createTargetPack(Card.Color color, GlobalUndo stack);

    /**
     * Create working pack , add to list for help using method add
     * @param CardPack from where are added cards to working pack
     * @param number number of added cards from card deck
     * @param stack undo operation
     * @return created working pack
     */
    public abstract CardStack createWorkingPack(CardDeck CardPack, int number, GlobalUndo stack);

    /**
     * create stacker for defermenting cards from card deck
     * @return created stacker
     */
    public abstract CardDeck createStacker();

    /**
     * load cards from file to array list
     * @param file name of input file
     * @return loaded ArrayList
     */
    public abstract ArrayList loadWorkingPack(String file);
    /**
     * load cards from file to array list
     * @param file name of input file
     * @return loaded ArrayList
     */
    public abstract ArrayList  loadTargetPack(String file);
    /**
     * load cards from file to array list
     * @param file name of input file
     * @return loaded ArrayList
     */
    public abstract ArrayList  loadCardDeck(String file);

    /**
     * erase all items from object list used for help
     */
    public abstract void clear_list();


}
