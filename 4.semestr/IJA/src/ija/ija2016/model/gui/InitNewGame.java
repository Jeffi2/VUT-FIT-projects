/**
 * IJA - project
 * @file: InitNewGame.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.gui;

import ija2016.model.classes.MyJLabel;
import ija2016.model.classes.PrepareGame;
import ija2016.model.interfaces.CardDeck;
import ija2016.model.interfaces.CardStack;

import javax.swing.*;
import javax.imageio.ImageIO;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.awt.*;

/**
 * Class represents one game gui, initialize all graphic components for one game
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public class InitNewGame {
    private JPanel mainPanel;
    private JPanel top;
    private JPanel center;
    private JPanel working1;
    private JPanel working2;
    private JPanel working3;
    private JPanel working4;
    private JPanel working5;
    private JPanel working6;
    private JPanel working7;
    private JPanel buttonHolder;
    private JLabel pack;
    private JLabel stacker;
    private JLabel targetClubs;
    private JLabel targetDiamonds;
    private JLabel targetHearts;
    private JLabel targetSpades;
    private JLabel informUser;
    private JButton undoButton;
    private JButton saveButton;
    private JButton loadButton;
    private JButton helpButton;
    private JButton exitGame;
    private PrepareGame game;
    private BufferedImage defaultStacker;
    private BufferedImage defaultTH;
    private BufferedImage defaultTC;
    private BufferedImage defaultTS;
    private BufferedImage defaultTD;
    private CardStack beforeStack;
    private CardDeck beforeDeck;
    private boolean beforeClick;
    private boolean beforeW;
    private boolean beforeS;
    private boolean beforeT;
    private GKCard beforeCard;
    private MainGuiFrame mainFrame;
    private boolean win;
    private InitNewGame thisGame;

    /**
     * Create a gui for game core
     * @param panel free graphic panel
     * @param game game core
     * @param frame main frame
     */
    public InitNewGame(JPanel panel, PrepareGame game, MainGuiFrame frame) {
        panel.setLayout(new BorderLayout());
        this.win = false;
        this.thisGame = this;
        this.mainFrame = frame;
        this.game = game;
        this.beforeClick = false;
        this.beforeW = false;
        this.top = new JPanel(new GridLayout(1, 7, 10, 10));
        this.top.setBackground(new Color(70, 139, 57));
        this.top.setVisible(true);
        this.mainPanel = panel;
        panel.add(this.top, BorderLayout.PAGE_START);
        this.informUser = new JLabel();
        this.informUser.setForeground(Color.white);
        panel.add(informUser, BorderLayout.PAGE_END);
        this.center = new JPanel(new GridLayout(1, 8, 0, 10));
        this.center.setBackground(new Color(70, 139, 57));
        panel.add(this.center, BorderLayout.CENTER);
        try {
            this.defaultStacker = ImageIO.read(new File("src/ija/ija2016/images/empty.png"));
            this.defaultTS = ImageIO.read(new File("src/ija/ija2016/images/TS.png"));
            this.defaultTC = ImageIO.read(new File("src/ija/ija2016/images/TC.png"));
            this.defaultTH = ImageIO.read(new File("src/ija/ija2016/images/TH.png"));
            this.defaultTD = ImageIO.read(new File("src/ija/ija2016/images/TD.png"));
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        CreateTopElements();
        CreateWorkingElements();
    }

    /**
     * Reprint working stack
     * @param working panel for printing
     * @param stack working stack data
     * @param numberOfWorking number of working stack for label
     */
    private void ChangeWorkingCards(JPanel working, CardStack stack, int numberOfWorking) {
        working.removeAll();
        int x = 10;
        if (mainFrame.freePanelsSize() == 3) {
            x = 70;
        }
        JLabel name = new JLabel("Working "+numberOfWorking);
        name.setBounds(x+5, 0, 90, 15);
        name.setForeground(Color.white);
        working.add(name, 0);
        int y = 20;
        int cardWidth = 75;
        int cardHeigth = 110;
        for (int i =stack.size(); i > 0 ; i--) {
            MyJLabel card = new MyJLabel((GKCard) stack.get(i-1), stack);
            card.setBounds(x, y, cardWidth, cardHeigth);
            y += 15;
            card.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (!card.getGKCard().isTurnedFaceUp()) return;
                    if (beforeClick) {
                        if (beforeW) {
                            card.getWorkingStack().moveMoreCards(beforeStack, beforeCard, card.getWorkingStack());
                            beforeW = false;
                            beforeS = false; //delete?
                            beforeT = false;
                            beforeClick = false;
                            informUser.setText("");
                            updateWorking();
                        }
                        else if (beforeS) {
                            beforeClick = false;
                            beforeW = false; //delete?
                            beforeS = false;
                            beforeT = false;
                            informUser.setText("");
                            card.getWorkingStack().moveOneCard(beforeDeck, card.getWorkingStack());
                            updateWorking();
                            updateStacker();
                        }
                        else if (beforeT) {
                            beforeClick = false;
                            beforeW = false; //delete?
                            beforeS = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            card.getWorkingStack().moveOneCard(beforeDeck, card.getWorkingStack());
                            updateT();
                            updateWorking();
                        }
                    }
                    else {
                        beforeClick = true;
                        beforeW = true;
                        beforeS = false;
                        informUser.setText("Choose where to move.");
                        beforeStack = card.getWorkingStack();
                        beforeCard = card.getGKCard();
                    }
                }
            });
            working.add(card, 0);
        }

    }

    /**
     * Checks if game is at the end
     */
    private void checkWin() {
        if ((game.getTDiamonds().size()) + (game.getTClubs().size())
                + (game.getTSpides().size()) + (game.getTHearts().size()) == 52) {
            win = true;
            undoButton.setEnabled(false);
            saveButton.setEnabled(false);
            loadButton.setEnabled(false);
            helpButton.setEnabled(false);
            center.setLayout(new FlowLayout());
            JLabel winLabel = new JLabel("Vyhrál jsi!");
            winLabel.setForeground(Color.white);
            winLabel.setFont(new Font("Serif", Font.BOLD, 32));
            center.add(winLabel);
        }
    }

    /**
     * reprint all working stacks
     */
    public void updateWorking() {
        ChangeWorkingCards(working1, game.getWorking1(), 1);
        ChangeWorkingCards(working2, game.getWorking2(), 2);
        ChangeWorkingCards(working3, game.getWorking3(), 3);
        ChangeWorkingCards(working4, game.getWorking4(), 4);
        ChangeWorkingCards(working5, game.getWorking5(), 5);
        ChangeWorkingCards(working6, game.getWorking6(), 6);
        ChangeWorkingCards(working7, game.getWorking7(), 7);
        mainPanel.revalidate();
        mainPanel.repaint();
    }

    /**
     * Create all working elements
     */
    private void CreateWorkingElements(){
        working1 = new JPanel();
        working1.setLayout(null);
        working1.setBackground(new Color(70, 139, 57));
        working1.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking1().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking1().moveMoreCards(beforeStack, beforeCard, game.getWorking1());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking1().moveOneCard(beforeDeck, game.getWorking1());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working1);
        ChangeWorkingCards(working1, game.getWorking1(), 1);

        working2 = new JPanel();
        working2.setLayout(null);
        working2.setBackground(new Color(70, 139, 57));
        working2.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking2().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking2().moveMoreCards(beforeStack, beforeCard, game.getWorking2());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking2().moveOneCard(beforeDeck, game.getWorking2());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working2);
        ChangeWorkingCards(working2, game.getWorking2(), 2);

        working3 = new JPanel();
        working3.setLayout(null);
        working3.setBackground(new Color(70, 139, 57));
        working3.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking3().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking3().moveMoreCards(beforeStack, beforeCard, game.getWorking3());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking3().moveOneCard(beforeDeck, game.getWorking3());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working3);
        ChangeWorkingCards(working3, game.getWorking3(), 3);

        working4 = new JPanel();
        working4.setLayout(null);
        working4.setBackground(new Color(70, 139, 57));
        working4.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking4().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking4().moveMoreCards(beforeStack, beforeCard, game.getWorking4());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking4().moveOneCard(beforeDeck, game.getWorking4());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working4);
        ChangeWorkingCards(working4, game.getWorking4(), 4);

        working5 = new JPanel();
        working5.setLayout(null);
        working5.setBackground(new Color(70, 139, 57));
        working5.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking5().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking5().moveMoreCards(beforeStack, beforeCard, game.getWorking5());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking5().moveOneCard(beforeDeck, game.getWorking5());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working5);
        ChangeWorkingCards(working5, game.getWorking5(), 5);

        working6 = new JPanel();
        working6.setLayout(null);
        working6.setBackground(new Color(70, 139, 57));
        working6.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking6().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking6().moveMoreCards(beforeStack, beforeCard, game.getWorking6());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking6().moveOneCard(beforeDeck, game.getWorking6());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working6);
        ChangeWorkingCards(working6, game.getWorking6(), 6);

        working7 = new JPanel();
        working7.setLayout(null);
        working7.setBackground(new Color(70, 139, 57));
        working7.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (!game.getWorking7().isEmpty()) {
                    return;
                }
                else {
                    if (beforeClick) {
                        if (beforeW) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking7().moveMoreCards(beforeStack, beforeCard, game.getWorking7());
                            updateWorking();
                        }
                        else if (beforeS || beforeT) {
                            beforeW = false; beforeS = false;
                            beforeT = false; beforeClick = false;
                            informUser.setText("");
                            game.getWorking7().moveOneCard(beforeDeck, game.getWorking7());
                            updateT();
                            updateStacker();
                            updateWorking();
                        }
                    }
                }
            }
        });
        center.add(working7);
        ChangeWorkingCards(working7, game.getWorking7(), 7);
    }

    /**
     * reprint stacker image
     */
    private void updateStacker() {
        if (!game.getStacker().isEmpty()) {
            GKCard topCard = (GKCard) game.getStacker().get();
            stacker.setIcon(new ImageIcon(topCard.getCardImage()));
        }
        else {
            stacker.setIcon(new ImageIcon(defaultStacker));
        }
    }

    /**
     * reprint deck image
     */
    private void updatePack() {
        if (!game.getCardPack().isEmpty()) {
            GKCard topCard = (GKCard) game.getCardPack().get();
            pack.setIcon(new ImageIcon(topCard.getCardImage()));
        }
        else {
            pack.setIcon(new ImageIcon(defaultStacker));
        }
    }

    /**
     * reprint targets images
     */
    private void updateT() {
        checkWin();
        if (!game.getTClubs().isEmpty()) {
            GKCard topCard = (GKCard) game.getTClubs().get();
            targetClubs.setIcon(new ImageIcon(topCard.getCardImage()));
        }
        else {
            targetClubs.setIcon(new ImageIcon(defaultTC));
        }

        if (!game.getTDiamonds().isEmpty()) {
            GKCard topCard = (GKCard) game.getTDiamonds().get();
            targetDiamonds.setIcon(new ImageIcon(topCard.getCardImage()));
        }
        else {
            targetDiamonds.setIcon(new ImageIcon(defaultTD));
        }

        if (!game.getTSpides().isEmpty()) {
            GKCard topCard = (GKCard) game.getTSpides().get();
            targetSpades.setIcon(new ImageIcon(topCard.getCardImage()));
        }
        else {
            targetSpades.setIcon(new ImageIcon(defaultTS));
        }

        if (!game.getTHearts().isEmpty()) {
            GKCard topCard = (GKCard) game.getTHearts().get();
            targetHearts.setIcon(new ImageIcon(topCard.getCardImage()));
        }
        else {
            targetHearts.setIcon(new ImageIcon(defaultTH));
        }
    }

    /**
     * create all top elements (deck, stacker, targets, buttons)
     */
    private void CreateTopElements() {
        try {
            // Create pack icon
            GKCard topPack = (GKCard) game.getCardPack().get();
            JPanel topSplit1 = new JPanel(new BorderLayout());
            topSplit1.setBackground(new Color(70, 139, 57));
            JLabel balik = new JLabel("Pack");
            balik.setForeground(Color.white);
            balik.setHorizontalAlignment(JLabel.CENTER);
            topSplit1.add(balik, BorderLayout.PAGE_START);
            this.pack = new JLabel(new ImageIcon(topPack.getCardImage()));
            this.pack.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    informUser.setText("");
                    CardDeck pack= game.getCardPack();
                    pack.moveOneCard(pack, game.getStacker());
                    updatePack();
                    updateStacker();
                    mainPanel.revalidate();
                }
            });
            topSplit1.add(pack, BorderLayout.CENTER);
            top.add(topSplit1);

            this.stacker = new JLabel(new ImageIcon(defaultStacker));
            JPanel topSplit2 = new JPanel(new BorderLayout());
            topSplit2.setBackground(new Color(70, 139, 57));
            JLabel stack = new JLabel("Stacker");
            stack.setForeground(Color.white);
            stack.setHorizontalAlignment(JLabel.CENTER);
            topSplit2.add(stack, BorderLayout.PAGE_START);
            stacker.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (game.getStacker().isEmpty()) return;
                    beforeClick = true;
                    beforeS = true;
                    beforeDeck = game.getStacker();
                    informUser.setText("Choose where to move.");
                }
            });
            topSplit2.add(stacker, BorderLayout.CENTER);
            top.add(topSplit2);

            //Button holder
            this.buttonHolder = new JPanel(new GridLayout(5, 1, 0, 0));
            this.buttonHolder.setBackground(new Color(70, 139, 57));
            this.undoButton = new JButton("Undo");
            this.undoButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (!game.getGlobalUndoStack().isEmpty()) {
                        game.getGlobalUndoStack().makeUndo();
                        updateT();
                        updateWorking();
                        updateStacker();
                        updatePack();
                        informUser.setText("Undo.");
                    }
                    else {
                        informUser.setText("You are at the start of game.");
                    }
                }
            });
            this.buttonHolder.add(undoButton);
            this.saveButton = new JButton("Save game");
            this.saveButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    new Thread(() -> {
                        SwingUtilities.invokeLater(() -> {
                            JFileChooser fileChooser = new JFileChooser();
                            fileChooser.setDialogTitle("Specify file to save");
                            int select = fileChooser.showSaveDialog(mainPanel);
                            if (select == JFileChooser.APPROVE_OPTION) {
                                informUser.setText(game.save(fileChooser.getSelectedFile().getAbsolutePath()));
                            }
                        });
                    }).start();
                }
            });
            this.buttonHolder.add(saveButton);
            this.loadButton = new JButton("Load game");
            this.loadButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    new Thread(() -> {
                        SwingUtilities.invokeLater(() -> {
                            JFileChooser fileChooser = new JFileChooser();
                            fileChooser.setDialogTitle("Specify file to load");
                            int select = fileChooser.showOpenDialog(mainPanel);
                            if (select == JFileChooser.APPROVE_OPTION) {
                                informUser.setText(game.load(fileChooser.getSelectedFile().getAbsolutePath()));
                                updatePack();
                                updateStacker();
                                updateT();
                                updateWorking();
                            }
                        });
                    }).start();
                }
            });

            this.buttonHolder.add(loadButton);

            this.helpButton = new JButton("Help");
            this.helpButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    informUser.setText(game.help());
                }
            });
            this.buttonHolder.add(helpButton);
            this.exitGame = new JButton("Exit game");
            this.exitGame.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    mainPanel.removeAll();
                    mainFrame.addFreePanel(mainPanel);
                    mainFrame.removeGame(thisGame);
                    if (mainFrame.freePanelsSize() == 4) {
                        System.exit(0);
                    }
                    if (mainFrame.freePanelsSize() == 3) {
                        mainFrame.backOneView();
                        updateWorking();
                        mainFrame.reprintGame();
                        mainFrame.revalidate();
                    }
                    mainPanel.revalidate();
                    mainFrame.revalidate();
                    mainFrame.repaint();
                }
            });
            this.buttonHolder.add(exitGame);
            top.add(buttonHolder);

            this.targetDiamonds = new JLabel(new ImageIcon(defaultTD));
            JPanel topSplit3 = new JPanel(new BorderLayout());
            topSplit3.setBackground(new Color(70, 139, 57));
            JLabel target1 = new JLabel("Target 1");
            target1.setForeground(Color.white);
            target1.setHorizontalAlignment(JLabel.CENTER);
            topSplit3.add(target1, BorderLayout.PAGE_START);
            targetDiamonds.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (win) return;
                    if (beforeClick) {
                        if (beforeS) {
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            game.getTDiamonds().moveOneCard(beforeDeck, game.getTDiamonds());
                            updateStacker();
                            updateT();
                        }
                        else if (beforeW) {
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            game.getTDiamonds().moveOneCard(beforeStack, game.getTDiamonds());
                            updateWorking();
                            updateT();
                        }
                    }
                    else {
                        if (!game.getTDiamonds().isEmpty()) {
                            informUser.setText("Choose where to move.");
                            beforeClick = true;
                            beforeT = true;
                            beforeDeck = game.getTDiamonds();
                        }
                    }
                }
            });
            topSplit3.add(targetDiamonds, BorderLayout.CENTER);
            top.add(topSplit3);

            this.targetClubs = new JLabel(new ImageIcon(defaultTC));
            JPanel topSplit4 = new JPanel(new BorderLayout());
            topSplit4.setBackground(new Color(70, 139, 57));
            JLabel target2 = new JLabel("Target 2");
            target2.setForeground(Color.white);
            target2.setHorizontalAlignment(JLabel.CENTER);
            topSplit4.add(target2, BorderLayout.PAGE_START);
            targetClubs.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (win) return;
                    if (beforeClick) {
                        if (beforeS) {
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            game.getTClubs().moveOneCard(beforeDeck, game.getTClubs());
                            updateStacker();
                            updateT();
                        }
                        else if (beforeW) {
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            game.getTClubs().moveOneCard(beforeStack, game.getTClubs());
                            updateWorking();
                            updateT();
                        }
                    }
                    else {
                        if (!game.getTClubs().isEmpty()) {
                            informUser.setText("Choose where to move..");
                            beforeClick = true;
                            beforeT = true;
                            beforeDeck = game.getTClubs();
                        }
                    }
                }
            });
            topSplit4.add(targetClubs, BorderLayout.CENTER);
            top.add(topSplit4);

            this.targetHearts = new JLabel(new ImageIcon(defaultTH));
            JPanel topSplit5 = new JPanel(new BorderLayout());
            topSplit5.setBackground(new Color(70, 139, 57));
            JLabel target3 = new JLabel("Target 3");
            target3.setForeground(Color.white);
            target3.setHorizontalAlignment(JLabel.CENTER);
            topSplit5.add(target3, BorderLayout.PAGE_START);
            targetHearts.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (win) return;
                    if (beforeClick) {
                        if (beforeS) {
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            game.getTHearts().moveOneCard(beforeDeck, game.getTHearts());
                            updateStacker();
                            updateT();
                        }
                        else if (beforeW) {
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            informUser.setText("");
                            game.getTHearts().moveOneCard(beforeStack, game.getTHearts());
                            updateWorking();
                            updateT();
                        }
                    }
                    else {
                        if (!game.getTHearts().isEmpty()) {
                            informUser.setText("Choose where to move.");
                            beforeClick = true;
                            beforeT = true;
                            beforeDeck = game.getTHearts();
                        }
                    }
                }
            });
            topSplit5.add(targetHearts, BorderLayout.CENTER);
            top.add(topSplit5);

            this.targetSpades = new JLabel(new ImageIcon(defaultTS));
            JPanel topSplit6 = new JPanel(new BorderLayout());
            topSplit6.setBackground(new Color(70, 139, 57));
            JLabel target4 = new JLabel("Target 4");
            target4.setForeground(Color.white);
            target4.setHorizontalAlignment(JLabel.CENTER);
            topSplit6.add(target4, BorderLayout.PAGE_START);
            targetSpades.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (win) return;
                    if (beforeClick) {
                        if (beforeS) {
                            informUser.setText("");
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            game.getTSpides().moveOneCard(beforeDeck, game.getTSpides());
                            updateStacker();
                            updateT();
                        }
                        else if (beforeW) {
                            informUser.setText("");
                            beforeClick = false;
                            beforeS = false;
                            beforeW = false; //delete?
                            beforeT = false;
                            game.getTSpides().moveOneCard(beforeStack, game.getTSpides());
                            updateWorking();
                            updateT();
                        }
                    }
                    else {
                        if (!game.getTSpides().isEmpty()) {
                            informUser.setText("Choose where to move.");
                            beforeClick = true;
                            beforeT = true;
                            beforeDeck = game.getTSpides();
                        }
                    }
                }
            });
            topSplit6.add(targetSpades, BorderLayout.CENTER);
            top.add(topSplit6);

        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
