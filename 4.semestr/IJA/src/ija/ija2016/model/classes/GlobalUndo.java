/**
 * IJA - project
 * @file: GlobalUndo.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import java.util.ArrayList;
/**
 * Global stack for each game, store all undo operations
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class GlobalUndo {
    private ArrayList<Commander.Invoker> undoStack;

    public GlobalUndo(){
        undoStack = new ArrayList<>();
    }

    /**
     * Add invoker to global stack
     * @param invoker actor that executed command
     */
    public void put(Commander.Invoker invoker){
        undoStack.add(0, invoker);
    }

    /**
     * Execute undo of the last command
     */
    public void makeUndo(){
        if (!undoStack.isEmpty()){
            Commander.Invoker temp = undoStack.get(0);
            temp.undo();
            undoStack.remove(0);
        }
    }

    /**
     * Remove all stored commands
     */
    public void removeAll() {
        this.undoStack.clear();
    }

    /**
     * Information if some command was executed
     * @return true if some command is stored in global undo stack
     */
    public boolean isEmpty() {
        return undoStack.isEmpty();
    }
}
