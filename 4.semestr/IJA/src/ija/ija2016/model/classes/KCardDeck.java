/**
 * IJA - project
 * @file: KCardDeck.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import ija2016.model.interfaces.Card;
import ija2016.model.interfaces.CardDeck;
import ija2016.model.interfaces.Help;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;

/**
 * Class represents standard card deck, defined in interface CardDeck
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class KCardDeck extends game_handler implements CardDeck , Help{
    protected  int size;
    protected ArrayList<Card> CardsList;
    protected final transient Commander.Invoker invoker = new Commander.Invoker();
    protected transient GlobalUndo undoStack;
    protected static int count=0;
    protected String name;

    public KCardDeck(int size) {
        this.CardsList = new ArrayList<Card>(size);
        count++;
        this.name="Stacker";
    }
    public KCardDeck(int size, GlobalUndo stack) {
        this.CardsList = new ArrayList<Card>(size);
        this.undoStack = stack;
        count++;
        this.name="Stacker";
    }

    @Override
    public void moveOneCard(CardDeck source, CardDeck dest){
        undoStack.put(invoker);
        invoker.execute(new Commander.Command() {
            @Override
            public void execute() {
                if (!source.isEmpty()) {
                    Card temp = source.pop();
                    temp.turnFaceUp();
                    dest.put(temp, false);
                } else {
                    int stackerSize = dest.size();
                    for (int i = 0; i < stackerSize; i++) {
                        Card tempCard = dest.pop();
                        tempCard.turnFaceDown();
                        source.put(tempCard, false);
                    }
                }
            }

            @Override
            public void undo() {
                if (!dest.isEmpty()) {
                    Card temp = dest.pop();
                    temp.turnFaceDown();
                    source.put(temp, false);
                }
                else {
                    int stackerSize = source.size();
                    for (int i = 0; i < stackerSize; i++) {
                        Card tempCard = source.pop();
                        tempCard.turnFaceUp();
                        dest.put(tempCard, false);
                    }
                }
            }
        });
    }

    /**
     * execute undo command
     */
    public void undo(){
        invoker.undo();
    }

    public void suffle() {
        if (this.CardsList.size() > 1) {
            long seed = System.nanoTime();
            Collections.shuffle(this.CardsList, new Random(seed));
        }
    }

    public Card pop() {
        if (this.CardsList.isEmpty()) return null;
        Card tmp = CardsList.get(0);
        this.CardsList.remove(0);
        return tmp;
    }

    public Card get() {
        if (this.CardsList.isEmpty()) return null;
        Card tmp = CardsList.get(0);
        return tmp;
    }

    public Card get(int index) { //test
        int prepocet = ((CardsList.size()-1) - index);
        if (index < 0 || index > (CardsList.size()-1)) return null;
        Card tmp = CardsList.get(index);
        return tmp;
    }

    public boolean isEmpty() {
        if (this.CardsList.isEmpty()) {
            return true;
        }
        return false;
    }

     public boolean put(Card card, boolean trash) {
         this.CardsList.add(0, card); // kdy vratit false?
         return true;
    }

    public int size() {
        return this.CardsList.size();
    }

    public void print() {
        for (int i = 0; i < this.CardsList.size(); i++) {
            Card tmp = this.CardsList.get(i);
            String face;
            if (tmp.isTurnedFaceUp()) {
                face = "Ukazana";
            }
            else {
                face = "Neukazana";
            }
            System.out.print(tmp + " " + face + "     ");
        }
    System.out.println();
    }
    public String name(){
        return this.name;
    }
    public boolean next(Card card){
        return false;
    }
    public CardDeck help(ArrayList list){

        for (int i = 0; i < list.size(); i++) { //for each stack
            Help item = (Help) list.get(i);
            if (this.isEmpty())
                return null;
            if (item.next(this.get(0)))
                return (CardDeck) item;
        }
        return null;
    }

    @Override
    public boolean save(String name) {

    if (fileOut == null)
            if (!game_handler.setfile(name))
                return false;
        try {

        out.writeObject(this.CardsList);

    }catch(IOException i) {
        i.printStackTrace();
        return false;
    }
        return true;
    }
    public void changelist(ArrayList<Card> new_list){
        CardsList=new_list;

    }
}

