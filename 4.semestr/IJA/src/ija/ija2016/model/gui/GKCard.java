/**
 * IJA - project
 * @file: GKCard.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.gui;

import ija2016.model.classes.KCard;
import ija2016.model.interfaces.Card;
import ija2016.model.interfaces.GCard;

import java.awt.image.BufferedImage;

/**
 * Class represents graphic implementation of one card
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class GKCard extends KCard implements java.io.Serializable,GCard {
    private transient BufferedImage cardImage;
    private transient BufferedImage backImage;

    public GKCard (Card.Color c, int value, BufferedImage valueImage, BufferedImage backImage) {
        super(c, value);
        this.cardImage = valueImage;
        this.backImage = backImage;
    }

    /**
     * Choose image of card depends if card is turned up/down
     * @return card image
     */
    public BufferedImage getCardImage() {
        if (this.isTurnedFaceUp()) {
            return cardImage;
        }
        else {
            return backImage;
        }
    }
    public void setImage(BufferedImage valueImage,BufferedImage Back){
        cardImage=valueImage;
        backImage=Back;
    }


}
