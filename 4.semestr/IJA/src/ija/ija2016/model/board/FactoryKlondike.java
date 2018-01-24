/**
 * IJA - project
 * @file: FactoryKlondike.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.board;

import ija2016.model.classes.*;
import ija2016.model.gui.GKCard;
import ija2016.model.interfaces.*;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.ArrayList;


/**
 * Class implementing factory pattern for creating cards or stacks
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class FactoryKlondike extends AbstractFactorySolitaire {
    static private FileInputStream fileIn;
    static private ObjectInputStream in;
    private ArrayList<Object> obj_list;

    public FactoryKlondike(){
        obj_list = new ArrayList<>(15);
    }
    public  void add(Object obj){
        obj_list.add(obj);
    }

    public ArrayList<Object> list(){
        return this.obj_list;
    }
    public void clear_list(){
        obj_list.clear();
    }
    private boolean setfile(String file){
       try {
           fileIn = new FileInputStream(file);
           in = new ObjectInputStream(fileIn);
           return true;
       }
       catch(IOException i) {
           i.printStackTrace();
           return false;
       }
    }
    public static void closefile(){
        if (in != null) {
            try {
                in.close();
                fileIn.close();
            } catch (IOException i) {
                i.printStackTrace();
            }
        }
        in=null;
        fileIn=null;
    }
    public ArrayList<Card> loadWorkingPack(String file){
        if (fileIn == null)
            if(!this.setfile(file))
                return null;
        try {
            ArrayList<Card> a = (ArrayList<Card>) in.readObject();

            return a;
        }catch(IOException i) {
            i.printStackTrace();
        }catch(ClassNotFoundException c) {
            c.printStackTrace();

        }
        return null;

    }
     public Card createCard(Card.Color color, int value) {
         if (value > 13 || value <= 0) return null;
         if (color != Card.Color.CLUBS && color != Card.Color.DIAMONDS && color != Card.Color.HEARTS && color != Card.Color.SPADES) return null;
         Card createdCard = new KCard(color, value);

         return createdCard;
     }

     public CardDeck createCardDeck(GlobalUndo stack) {
        CardDeck standartDeck = new KCardDeck(52, stack);
        try {
            BufferedImage backImage = ImageIO.read(new File("src/ija/ija2016/images/back.png"));
            for (int i = 1; i <= 13; i++) {
                standartDeck.put(new GKCard(Card.Color.CLUBS, i, ImageIO.read(new File("src/ija/ija2016/images/"+i+"C"+"-M.png")), backImage), false);
            }
            for (int i = 1; i <= 13; i++) {
                standartDeck.put(new GKCard(Card.Color.DIAMONDS, i, ImageIO.read(new File("src/ija/ija2016/images/"+i+"D"+"-M.png")), backImage), false);
            }
            for (int i = 1; i <= 13; i++) {
                standartDeck.put(new GKCard(Card.Color.HEARTS, i, ImageIO.read(new File("src/ija/ija2016/images/"+i+"H"+"-M.png")), backImage), false);
            }
            for (int i = 1; i <= 13; i++) {
                standartDeck.put(new GKCard(Card.Color.SPADES, i, ImageIO.read(new File("src/ija/ija2016/images/"+i+"S"+"-M.png")), backImage), false);
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        return standartDeck;
     }
     public ArrayList<Card> loadCardDeck(String file){
         if (fileIn == null)
             if (!this.setfile(file))
                 return null;
         try {

             ArrayList<Card> a = (ArrayList<Card>) in.readObject();

             return a;
         }catch(IOException i) {
             i.printStackTrace();
             return null;

         }catch(ClassNotFoundException c) {
             c.printStackTrace();
             return null;

         }



     }
     public CardDeck createTargetPack(Card.Color color, GlobalUndo stack){
        CardDeck createdDeck = new KTarget(0, color, stack);
        this.add(createdDeck);
        return createdDeck;
     }
    public ArrayList<Card> loadTargetPack(String file){
        if (fileIn == null)
            this.setfile(file);
         try {
            ArrayList<Card> a = (ArrayList<Card>) in.readObject();
            return a;
        }catch(IOException i) {
            i.printStackTrace();

        }catch(ClassNotFoundException c) {
            c.printStackTrace();
        }
        return null ;

    }

    public CardStack createWorkingPack(CardDeck CardPack, int numberOfCards, GlobalUndo stack){
        CardStack createdStack = new KWorking(numberOfCards, stack);
        for (int i = 0; i < numberOfCards; i++){
            Card temp = CardPack.pop();
            createdStack.put(temp, true);
        }
        createdStack.get().turnFaceUp();
        this.add(createdStack);
        return createdStack;
    }

    public CardDeck createStacker(){
        CardDeck createdDeck = new KCardDeck(0);
        this.add(createdDeck);
        return createdDeck;
    }
}
