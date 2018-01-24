/**
 * IJA - project
 * @file: KTarget.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import ija2016.model.interfaces.Card;
import ija2016.model.interfaces.CardDeck;
import ija2016.model.interfaces.Help;


/**
 * Class represents target stack for cards
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class KTarget extends KCardDeck implements Help {
    private Card.Color packColor;
    private static int count=0;

    /**
     * Constructor for normal target stack
     * @param size size of stack
     * @param color color of stack
     */
    public KTarget(int size, Card.Color color) {
        super(size);
        this.packColor = color;

        this.name="Target: "+((count%4)+1);
        count++;
    }

    /**
     * Constructor with global undo stack for revert operations
     * @param size size of stack
     * @param color color of stack
     * @param stack global undo stack
     */
    public KTarget(int size, Card.Color color, GlobalUndo stack) {
        super(size);
        this.packColor = color;
        this.undoStack = stack;

        this.name="Target: "+((count%4)+1);
        count++;
    }

    @Override
    public void moveOneCard(CardDeck source, CardDeck dest){
        invoker.execute(new Commander.Command() {
            boolean turnedUnder = false;
            @Override
            public void execute() {
                if (!source.isEmpty()){
                    Card temp = source.get();
                    if (temp.isTurnedFaceUp()){
                        if (dest.put(temp, false)) {
                            undoStack.put(invoker);
                            source.pop();
                            if (source.size() > 0){
                                temp = source.get();
                                if (!temp.isTurnedFaceUp()){
                                    turnedUnder = true;
                                    temp.turnFaceUp();
                                }
                            }
                        }

                    }
                }
            }

            @Override
            public void undo() {
                if (turnedUnder) {
                    Card temp = source.get();
                    temp.turnFaceDown();
                }
                Card temp = dest.pop();
                source.put(temp, true);
            }
        });
    }

    @Override
    public boolean put(Card card, boolean trash) {
        if (CardsList.isEmpty()) {
            if (card.value() == 1 && card.color() == this.packColor) {
                this.CardsList.add(0, card);
                return true;
            }
        }
        else {

            if (card.color() == this.packColor && CardsList.get(0).value()+1 == card.value()) {
                this.CardsList.add(0, card);
                return true;
            }
        }
        return false;
    }
    @Override
    public boolean next(Card card) {
        if (CardsList.isEmpty()) {
            if (card.value() == 1 && card.color() == this.packColor) {
                return true;
            }
        }
        else {
            if (card.color() == this.packColor && CardsList.get(0).value()+1 == card.value()) {
                return true;
            }
        }
        return false;
    }
    //pro každemu stacku ve hře se pokusime nacpat naši posledni kartu pokud uspějeme
    // vratime baliček na ktery jde vložit

}
