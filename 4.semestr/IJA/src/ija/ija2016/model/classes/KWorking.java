/**
 * IJA - project
 * @file: KWorking.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import ija2016.model.interfaces.Card;
import ija2016.model.interfaces.CardDeck;
import ija2016.model.interfaces.CardStack;
import ija2016.model.interfaces.Help;

import java.util.ArrayList;

/**
 * Class represents working stack for moving cards
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class KWorking extends KCardStack implements CardDeck , Help{
    protected static int count=0;

    /**
     * Create basic working stack
     * @param size size of stack
     */
    public KWorking(int size){
        super(size);
        this.name="Working "+((count%7)+1);
        count++;

    }

    /**
     * Create working stack with global undo
     * @param size size of stack
     * @param stack global undo stack
     */
    public KWorking(int size, GlobalUndo stack) {
        super(size);
        undoStack = stack;
        this.name="Working "+((count%7)+1);
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
        if (trash) {
            return super.put(card, false);
        }
        if (CardsList.isEmpty()) {
            if (card.value() == 13) {
                this.CardsList.add(0, card);
                return true;
            }
        }
        else {
            if (!card.color().similarColorTo(CardsList.get(0).color()) && CardsList.get(0).value()-1 == card.value()) {
                this.CardsList.add(0, card);
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean put(CardStack stack, boolean trash) {
        if (trash) super.put(stack, false);
        KCardStack test = (KCardStack) stack;
        if (test.size() == 0) return false;
        if ( (CardsList.isEmpty() && (test.get(test.size()-1).value() == 13))
            || (!CardsList.isEmpty() && (test.get(test.size()-1).value()+1) == CardsList.get(0).value() && !test.get(test.size()-1).color().similarColorTo(CardsList.get(0).color()))) {

            if (test.size() >= 2){
              for (int i = 0; i < test.size()-1; ++i) {
                if (test.get(i).    similarColorTo(test.get(i+1)) || test.get(i).value()+1 != test.get(i+1).value() )  { return false; }
              }
            }
            for (int i = (test.size()); i > 0; --i) {
                int goodIndex = i;
                goodIndex--;
                this.CardsList.add(0, stack.get(goodIndex));
            }
            for (int i = (test.size()); i > 0; --i) {
                int goodIndex = i;
                goodIndex--;
                test.remove();
            }
            return true;
    }
        return false;
    }
@Override
    public boolean next(Card card) {
        if (CardsList.isEmpty()) {
            if (card.value() == 13) {
                return true;
            }
        }
        else {
            if (!card.color().similarColorTo(CardsList.get(0).color()) && CardsList.get(0).value()-1 == card.value()) {
                return true;
            }
        }
        return false;
    }
    @Override //potřeba iterovat working stackem
    public CardDeck help(ArrayList list){
        for (int i = 0; i < list.size(); i++) { //for each stack
            Help item = (Help) list.get(i);
            if (this.isEmpty())
                return null;
            for (int j = 0; j < this.size(); j++) {
                if (!this.get(j).isTurnedFaceUp())
                    break;
                if (item.next(this.get(j)))
                    return (CardDeck) item;
            }
        }
        return null;
    }
    public void changelist(ArrayList<Card> new_list){
        CardsList=new_list;

    }

}

