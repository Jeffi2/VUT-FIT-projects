/**
 * IJA - project
 * @file: MyLabel.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import ija2016.model.gui.GKCard;
import ija2016.model.interfaces.CardStack;

import javax.swing.*;

/**
 * Class extends JLabel and store info for card print
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class MyJLabel extends JLabel {
    private GKCard card;
    private CardStack workingStack;

    /**
     * Create a label that is graphic represent of card
     * @param card Card
     * @param stack Stack where card is
     */
    public MyJLabel(GKCard card, CardStack stack) {
        super(new ImageIcon(card.getCardImage()));
        this.card = card;
        this.workingStack = stack;
    }

    /**
     * Getter for card
     * @return card
     */
    public GKCard getGKCard() {
        return card;
    }

    /**
     * Getter for stack
     * @return stack where card is
     */
    public CardStack getWorkingStack() {
        return workingStack;
    }
}
