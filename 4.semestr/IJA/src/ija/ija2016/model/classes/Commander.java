/**
 * IJA - project
 * @file: Commander.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.classes;


import java.util.ArrayList;
import java.util.List;
/**
 * Class implementing Command pattern for undo operation
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class Commander {
    /**
     * Represents single command for undo
     */
    public static interface Command{
        /**
         * execute behavior of command
         */
        public void execute();

        /**
         * undo behavior of command
         */
        public void undo();
    }

    /**
     * Class represents actor that execute some command for undo
     */
    public static class Invoker{
        List<Command> commands = new ArrayList<>();

        /**
         * Execute command that could be revert
         * @param cmd single move command
         */
        public void execute(Command cmd) {
            commands.add(0, cmd);
            cmd.execute();
        }

        /**
         * Execute undo command from Command list
         */
        public void undo() {
            if (commands.size() > 0){
                Command cmd = commands.remove(0);
                cmd.undo();
            }
        }
    }
}
