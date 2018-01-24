/**
 * IJA - project
 * @file: game_handler.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;

/**
 * Class handling save and load game
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class game_handler {

    protected static FileOutputStream fileOut =null;
    protected static ObjectOutputStream out =null;

    /**
     * create file descriptor, object out stream and set static attributes
     * @param name of file for write data
     * @return true if can be opened or created
     */
    protected static boolean setfile(String name){
        try {
            fileOut = new FileOutputStream(name);
             out = new ObjectOutputStream(fileOut);
             return true;
        }catch(IOException i) {
            i.printStackTrace();
            return false;
        }
    }

    /**
     * close file descriptor and object stream
     */
    public static void closefile(){
        try {
            out.close();
            fileOut.close();
            out=null;
            fileOut=null;
        }catch(IOException i) {
            i.printStackTrace();
        }
    }

    /**
     * Saves stack state
     * @param name absolute file path
     * @return true when save is ok
     */
    public boolean save(String name){
       if (fileOut == null)
           game_handler.setfile(name);
        try {

            out.writeObject(this);

        }catch(IOException i) {
            i.printStackTrace();
            return false;
        }
        return true;
    }

}
