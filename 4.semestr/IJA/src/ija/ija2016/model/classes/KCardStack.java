/**
 * IJA - project
 * @file: KCardStack.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import ija2016.model.interfaces.Card;
import ija2016.model.interfaces.CardDeck;
import ija2016.model.interfaces.CardStack;

/**
 * Class represents basic card stack, defined in interface CardStack
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class KCardStack extends KCardDeck implements CardStack {

    public boolean load(String file){
        return true;
    }
    public KCardStack(int size) {
        super (size);
    }

    public void moveMoreCards(CardStack source, Card card, CardStack dest){
        invoker.execute(new Commander.Command() {
            boolean turnedUnder = false;
            @Override
            public void execute() {
                if (!source.isEmpty()){
                    if (card.isTurnedFaceUp()) {
                        CardStack temp = source.pop(card);
                        if (dest.put(temp, false)){
                            undoStack.put(invoker);
                            if (!source.isEmpty()){
                                Card top = source.get();
                                if (!top.isTurnedFaceUp()){
                                    turnedUnder = true;
                                    top.turnFaceUp();
                                }
                            }
                        }
                        else {
                            source.put(temp, true);
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
                CardStack temp = dest.pop(card);
                source.put(temp, true);
            }
        });
    }

    public boolean put(CardStack stack, boolean trash) {
        KCardStack test = (KCardStack) stack;
        if (!stack.isEmpty()) {

            for (int i = (test.size()); i > 0; --i) {
                int goodIndex = i;
                goodIndex--;
                this.CardsList.add(0, stack.get(goodIndex));
            }
            for (int i = (test.size()); i > 0; --i) {
                test.remove();
            }
            return true;
        }
        return false;
    }

    /**
     * remove first card from stack
     */
    protected void remove() {
        this.CardsList.remove(0);
    }

    public CardStack pop(Card card) {

        int index = -1;
        boolean isFound = false;
        for (int i = 0; i < CardsList.size(); i++) {
            if (CardsList.get(i).color() == card.color() && CardsList.get(i).value() == card.value() ) {
                if (isFound) break;
                index = i;
                isFound = true;
            }
        }

        KCardStack removedCards;
        if (index == -1) {
            return null;
        }
        else {
            index++;
            removedCards = new KCardStack(index);
            if (index == 0) {
                removedCards.add(CardsList.get(0));
                CardsList.remove(0);
            }
            for (int i = 0; i < index; i++) {
                removedCards.add(CardsList.get(0));
                CardsList.remove(0);
            }
        }
        return removedCards;
    }

    private void add(Card c) {
        this.CardsList.add(c); // tento 0 index ovlivnuje jestli budou smazane karty z takeFrom vzestupne nebo sestupne.
    }

    public Card get(int i) {
        return this.CardsList.get(i);
    }

    @Override
    public boolean equals(Object obj) {
        CardStack tested = (CardStack) obj;
        if (tested == null) return false;
        if (CardsList.size() != tested.size())
            return false;
        else {
            boolean setRes = true;
            for (int i = 0; i < tested.size(); i++) {
                if (CardsList.get(i).color() != tested.get(i).color() || CardsList.get(i).value() != tested.get(i).value()) {
                    setRes = false;
                }
            }
            return setRes;
        }
    }
}
