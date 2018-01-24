/**
 * IJA - project
 * @file: PrepareGame.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import ija2016.model.board.AbstractFactorySolitaire;
import ija2016.model.board.FactoryKlondike;
import ija2016.model.interfaces.Card;
import ija2016.model.interfaces.CardDeck;
import ija2016.model.interfaces.CardStack;

import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

/**
 * Class represents one game core, initialize all stacks and cards
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class PrepareGame {
    private GlobalUndo undoStack;
    private AbstractFactorySolitaire factory;
    private CardDeck cardPack;
    private CardStack working1;
    private CardStack working2;
    private CardStack working3;
    private CardStack working4;
    private CardStack working5;
    private CardStack working6;
    private CardStack working7;
    private CardDeck TClubs;
    private CardDeck TDiamonds;
    private CardDeck THearts;
    private CardDeck TSpides;
    private CardDeck stacker;

    /**
     * Create and initialize game core
     */
    public PrepareGame() {
        this.undoStack = new GlobalUndo();
        this.factory = new FactoryKlondike();
        this.cardPack = factory.createCardDeck(this.undoStack);
        this.cardPack.suffle();
        this.stacker = factory.createStacker();
        this.TDiamonds = factory.createTargetPack(Card.Color.DIAMONDS, undoStack);
        this.TClubs = factory.createTargetPack(Card.Color.CLUBS, undoStack);
        this.THearts = factory.createTargetPack(Card.Color.HEARTS, undoStack);
        this.TSpides = factory.createTargetPack(Card.Color.SPADES, undoStack);
        this.working1 = factory.createWorkingPack(this.cardPack, 1 , undoStack);
        this.working2 = factory.createWorkingPack(this.cardPack, 2 , undoStack);
        this.working3 = factory.createWorkingPack(this.cardPack, 3 , undoStack);
        this.working4 = factory.createWorkingPack(this.cardPack, 4 , undoStack);
        this.working5 = factory.createWorkingPack(this.cardPack, 5 , undoStack);
        this.working6 = factory.createWorkingPack(this.cardPack, 6 , undoStack);
        this.working7 = factory.createWorkingPack(this.cardPack, 7 , undoStack);

    }

    /**
     * Save game data
     * @param file absolute file path
     * @return inform message for user
     */
    public String save(String file){
        if (!this.cardPack.save(file))
            return "ERROR";
        this.working1.save(file);
        this.working2.save(file);
        this.working3.save(file);
        this.working4.save(file);
        this.working5.save(file);
        this.working6.save(file);
        this.working7.save(file);
        this.TClubs.save(file);
        this.TDiamonds.save(file);
        this.THearts.save(file);
        this.TSpides.save(file);
        this.stacker.save(file);
        game_handler.closefile();

        return "SAVED";
    }

    /**
     * Set images to cards
     * @param c card
     */
    private void setter(Card c) {
        try {
            if (c.color() == Card.Color.CLUBS)
                c.setImage(ImageIO.read(new File("src/ija/ija2016/images/" + c.value() + "C" + "-M.png")), ImageIO.read(new File("src/ija/ija2016/images/back.png")));

            if (c.color() == Card.Color.HEARTS)
                c.setImage(ImageIO.read(new File("src/ija/ija2016/images/" + c.value() + "H" + "-M.png")), ImageIO.read(new File("src/ija/ija2016/images/back.png")));

            if (c.color() == Card.Color.SPADES)
                c.setImage(ImageIO.read(new File("src/ija/ija2016/images/" + c.value() + "S" + "-M.png")), ImageIO.read(new File("src/ija/ija2016/images/back.png")));

            if (c.color() == Card.Color.DIAMONDS)
                c.setImage(ImageIO.read(new File("src/ija/ija2016/images/" + c.value() + "D" + "-M.png")), ImageIO.read(new File("src/ija/ija2016/images/back.png")));

        }catch(IOException e){
            e.printStackTrace();
        }
    }

    /**
     * Load game data from file
     * @param file absolute file path
     * @return inform message for user
     */
    public String load(String file){


        this.factory = new FactoryKlondike();
        ArrayList tmp =factory.loadCardDeck(file);
        if (tmp == null) {
            FactoryKlondike.closefile();
            return "ERROR";
        }
        this.undoStack.removeAll();
        this.cardPack.changelist(tmp);
        this.working1.changelist( factory.loadWorkingPack(file));
        this.working2.changelist( factory.loadWorkingPack(file));
        this.working3.changelist( factory.loadWorkingPack(file));
        this.working4.changelist( factory.loadWorkingPack(file));
        this.working5.changelist( factory.loadWorkingPack(file));
        this.working6.changelist( factory.loadWorkingPack(file));
        this.working7.changelist( factory.loadWorkingPack(file));
        this.TClubs.changelist(factory.loadTargetPack(file));
        this.TDiamonds.changelist(factory.loadTargetPack(file));
        this.THearts.changelist(factory.loadTargetPack(file));
        this.TSpides.changelist(factory.loadTargetPack(file));
        this.stacker.changelist(factory.loadCardDeck(file));

        this.factory.clear_list();
        this.factory.add(this.working1);
        this.factory.add(this.working2);
        this.factory.add(this.working3);
        this.factory.add(this.working4);
        this.factory.add(this.working5);
        this.factory.add(this.working6);
        this.factory.add(this.working7);
        this.factory.add(this.TClubs);
        this.factory.add(this.TDiamonds);
        this.factory.add(this.THearts);
        this.factory.add(this.TSpides);
        this.factory.add(this.stacker);

            for(int i = 0; i<this.cardPack.size() ; i++) {
                this.setter (this.cardPack.get(i));
            }
            for(int i = 0; i<this.working1.size() ; i++) {
                this.setter (this.working1.get(i));
            }
            for(int i = 0; i<this.working2.size() ; i++)
                this.setter (this.working2.get(i));
            for(int i = 0; i<this.working3.size() ; i++)
                this.setter (this.working3.get(i));
            for(int i = 0; i<this.working4.size() ; i++)
                this.setter (this.working4.get(i));
            for(int i = 0; i<this.working5.size() ; i++)
                this.setter (this.working5.get(i));
            for(int i = 0; i<this.working6.size() ; i++)
                this.setter (this.working6.get(i));
            for(int i = 0; i<this.working7.size() ; i++)
                this.setter (this.working7.get(i));
            for(int i = 0; i<this.TClubs.size() ; i++)
                this.setter (this.TClubs.get(i));
            for(int i = 0; i<this.TDiamonds.size() ; i++)
                this.setter (this.TDiamonds.get(i));
            for(int i = 0; i<this.TSpides.size() ; i++)
                this.setter (this.TSpides.get(i));
            for(int i = 0; i<this.THearts.size() ; i++)
                this.setter (this.THearts.get(i));
            for(int i = 0; i<this.stacker.size() ; i++)
                this.setter (this.stacker.get(i));


        FactoryKlondike.closefile();
        return "LOAD";

    }

    /**
     * Method for help
     * @return message with help
     */
    public String help(){
        String help;
        ArrayList all_stacks=this.factory.list();
        for (int i = 0; i < all_stacks.size(); i++){
            CardDeck tmp=(CardDeck) all_stacks.get(i);
            CardDeck item =tmp.help(all_stacks);
            if (item != null){
                return "From: "+tmp.name()+"  To: "+item.name();
            }
        }
        if (this.cardPack.isEmpty())
            return "Nothing..";
        else
            return "Click on Pack.";
    }
    public GlobalUndo getGlobalUndoStack() {
        return undoStack;
    }

    public CardDeck getCardPack() {
        return cardPack;
    }

    public CardStack getWorking1() {
        return working1;
    }

    public CardStack getWorking2() {
        return working2;
    }

    public CardStack getWorking3() {
        return working3;
    }

    public CardStack getWorking4() {
        return working4;
    }

    public CardStack getWorking5() {
        return working5;
    }

    public CardStack getWorking6() {
        return working6;
    }

    public CardStack getWorking7() {
        return working7;
    }

    public CardDeck getTClubs() {
        return TClubs;
    }

    public CardDeck getTDiamonds() {
        return TDiamonds;
    }

    public CardDeck getTHearts() {
        return THearts;
    }

    public CardDeck getTSpides() {
        return TSpides;
    }

    public CardDeck getStacker() {
        return stacker;
    }
}