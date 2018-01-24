/**
 * IJA - project
 * @file: MainGuiFrame.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.gui;

import ija2016.model.classes.PrepareGame;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;

/**
 * Class represents main graphic frame, starts new games
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class MainGuiFrame extends JFrame {
    private JPanel panel1;
    private JPanel panel2;
    private JPanel panel3;
    private JPanel panel4;
    private GridLayout mainLayout;
    private JMenuBar menu;
    private JMenu exit;
    private JMenu newGame;
    private ArrayList<JPanel> freePanels;
    private ArrayList<InitNewGame> games;

    /**
     * Create one main gui frame
     */
    public MainGuiFrame() {
        super("Solitaire Klondike - xjanus08 / xjudap00");
        this.freePanels = new ArrayList<>();
        this.games = new ArrayList<>();
        this.mainLayout = new GridLayout(1, 2, 0, 0);
        setLayout(this.mainLayout);
        setVisible(true);
        //setResizable(false); THIS IS WAS THE BUG!!! (2h of debuging)
        //icon
        try {
            ImageIcon image = new ImageIcon("src/ija/ija2016/images/icon.png");
            setIconImage(image.getImage());
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        this.menu = new JMenuBar();
        this.newGame = new JMenu("Add game");
        this.newGame.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (freePanels.size() == 3){
                    splitView();
                    createGame(getFreePanel());
                    reprintGame();
                    revalidate();
                }
                else if (!isFreePanel()) {
                    JOptionPane.showMessageDialog(null, "You can play only 4 games. Exit some game.","Game warning!", JOptionPane.INFORMATION_MESSAGE);
                }
                else {
                    createGame(getFreePanel());
                    revalidate();
                }
            }
        });
        this.menu.add(newGame);
        this.exit = new JMenu("Exit");
        exit.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                System.exit(0);
            }
        });
        this.menu.add(exit);
        setJMenuBar(menu);
        pack();
        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        this.panel1 = new JPanel();
        panel1.setBackground(Color.black);
        panel1.setBorder(BorderFactory.createEmptyBorder(3,3,3,3));
        add(panel1);
        this.panel2 = new JPanel();
        panel2.setBackground(Color.black);
        panel2.setBorder(BorderFactory.createEmptyBorder(3,3,3,3));
        this.panel3 = new JPanel();
        panel3.setBackground(Color.black);
        panel3.setBorder(BorderFactory.createEmptyBorder(3,3,3,3));
        this.panel4 = new JPanel();
        panel4.setBackground(Color.black);
        panel4.setBorder(BorderFactory.createEmptyBorder(3,3,3,3));
        freePanels.add(0, panel2);
        freePanels.add(0, panel3);
        freePanels.add(0, panel4);
        createGame(panel1);
    }

    /**
     * Create game core and init single game gui
     * @param panel panel for game
     */
    private void createGame(JPanel panel) {
        PrepareGame gameCore = new PrepareGame();
        InitNewGame gameGui = new InitNewGame(panel, gameCore, this);
        this.games.add(0, gameGui);
    }

    /**
     * Reprint game on change view
     */
    public void reprintGame() {
        for (int i = 0; i < this.games.size(); i++) {
            InitNewGame game = this.games.get(i);
            game.updateWorking();
        }
    }

    /**
     * Remove game from list of playing games
     * @param game game gui
     */
    public void removeGame(InitNewGame game) {
        int index = -1;
        for (int i = 0; i < this.games.size(); i++) {
            InitNewGame tmp = this.games.get(i);
            if (tmp == game) {
                index = i;
            }
        }
        if (index == -1) return;
        this.games.remove(index);
    }

    /**
     * Split one frame into four
     */
    public void splitView(){
        mainLayout.setRows(2);
        mainLayout.setColumns(2);
        setLayout(mainLayout);
        for (int i = 2; i > -1; i--) {
            add(freePanels.get(i));
        }
    }

    /**
     * From four frames back to one
     */
    public void backOneView(){
        mainLayout.setRows(1);
        mainLayout.setColumns(2);
        setLayout(mainLayout);
        for (int i = 0; i < 3; i++) {
            remove(freePanels.get(i));
        }
        revalidate();
        repaint();
    }

    /**
     * Add panel into free panels list
     * @param panel gui panel
     */
    public void addFreePanel(JPanel panel) {
        this.freePanels.add(0, panel);
    }

    /**
     * Get some free panel
     * @return panel for new game
     */
    public JPanel getFreePanel() {
        if (this.freePanels.isEmpty()) {
            return null;
        }
        return this.freePanels.remove(freePanels.size()-1);
    }

    /**
     * How much free space for new games is
     * @return count of used panels
     */
    public int freePanelsSize(){
        return freePanels.size();
    }

    /**
     * Check if there is space for some new game
     * @return true when max three games are playing
     */
    public boolean isFreePanel(){
        return !freePanels.isEmpty();
    }

    public JPanel getPanel1() {
        return panel1;
    }

    public JPanel getPanel2() {
        return panel2;
    }

    public JPanel getPanel3() {
        return panel3;
    }

    public JPanel getPanel4() {
        return panel4;
    }
}
