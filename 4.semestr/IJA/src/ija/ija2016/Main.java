/**
 * IJA - project
 * @file: Main.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016;

import ija2016.model.gui.MainGuiFrame;

/**
 * Main class starts the application
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class Main {
    /**
     * Starts gui and game core
     * @param args program arguments
     */
    public static void main(String[] args) {
        MainGuiFrame mainFrame = new MainGuiFrame();
        mainFrame.setSize(1440, 900);
    }
}
