/**
 * IJA - project
 * @file: KCard.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import java.awt.image.BufferedImage;
import java.util.Objects;
import ija2016.model.interfaces.Card;

/**
 * Class represents basic card, defined in interface Card
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class KCard implements Card {
    private int value;
    private Card.Color color;
    private boolean face = false; //Should be in constructor?

    //konstruktor
    public KCard (Card.Color c, int value) {
        this.color = c;
        this.value = value;
    }

    //vraci barvu
    public Card.Color color() {
        return this.color;
    }

    //vraci value
    public int value () {
        return this.value;
    }

    public int compareValue(Card c) {
        return (this.value() - c.value());
    }

    public boolean isTurnedFaceUp() {
        return this.face;
    }

    public boolean similarColorTo(Card c) {
        return (c.color().similarColorTo(this.color()));
    }

    public boolean similarCard(Card c){
        return (c.value() == value) && similarColorTo(c);
    }

    public boolean turnFaceUp() {
        if (this.face) {
            return false;
        }
        else {
            this.face = true;
            return true;
        }
    }

    public boolean turnFaceDown(){
        if (this.face) {
            this.face = false;
            return true;
        }
        else {
            return false;
        }
    }

    //@Override  equals, hashcode

    @Override
    public String toString() {
        switch (this.value) {
            case 11:
                return "J(" + this.color + ")";

            case 12:
                return "Q(" + this.color + ")";

            case 13:
                return "K(" + this.color + ")";
        }
        return this.value + "(" + this.color + ")";
    }

    @Override
    public boolean equals(Object obj) {
        Card tested = (Card) obj;
        if (tested == null) return false;
        return (this.value() == tested.value() && this.color() == tested.color());
    }

    @Override
    public int hashCode() {
        return Objects.hash(this.value, this.color);
    }

    public void setImage(BufferedImage valueImage, BufferedImage BackImage){
    }

}
