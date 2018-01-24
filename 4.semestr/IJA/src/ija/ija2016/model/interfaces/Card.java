/**
 * IJA - project
 * @file: Card.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.interfaces;


import java.awt.image.BufferedImage;

/**
 * Class represents interface of card
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public interface Card extends java.io.Serializable {
    enum Color {
        CLUBS("C"), DIAMONDS("D"), HEARTS("H"), SPADES("S");

        private final String setter;

        //construct for enum
        Color(final String setter) {
            this.setter = setter;
        }

        @Override
        public String toString() {
            return setter;
        }

        public boolean similarColorTo(Card.Color c) {
            if ((c.toString().equals(Color.CLUBS.toString()) ||  c.toString().equals(Color.SPADES.toString()))
                    && (setter.equals(CLUBS.toString()) || setter.equals(SPADES.toString())) ) {
                return true;
            }
            if ((c.toString().equals(Color.DIAMONDS.toString()) ||  c.toString().equals(Color.HEARTS.toString()))
                    && (setter.equals(DIAMONDS.toString()) || setter.equals(HEARTS.toString()) )) {
                return true;
            }
            return false;
        }

    }

    /**
     *
     * @return card color
     */
    Card.Color color();

    /**
     * campare value of card
     * @param c card used for comparing with actual card
     * @return diff int value
     */
    int compareValue(Card c);

    /**
     * check whether card is face up
     * @return return true if face up
     */
    boolean isTurnedFaceUp();

    /**
     * check if card c has same type (red/black) as card which invoke method
     * @param c tested card
     * @return true if color is similar
     */
    boolean similarColorTo(Card c);

    /**
     * if card is face down , value will change to faceUP
     * @return true if change value
     */
    boolean turnFaceUp();

    /**
     * if card is face down , value will change to faceDown
     * @return true if change value
     */
    boolean turnFaceDown();

    /**
     *
     * @return value of card
     */
    int value();

    /**
     * check if card has same type(red/black) like card c
     * @param c card
     * @return true if similar
     */
    boolean similarCard(Card c);

    /**
     * sets card images after load from file
     * @param valueImage front card image
     * @param BackImage back card image
     */
    void setImage(BufferedImage valueImage, BufferedImage BackImage);


}
