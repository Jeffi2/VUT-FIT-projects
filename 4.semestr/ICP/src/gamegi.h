/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing one game in GUI
 * It contains all stacks, button and layout of the game
 *
 */

#ifndef GAMEGI_H
#define GAMEGI_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

class GameGI;

#include "mainwidget.h"
#include "GameCore.h"
#include "wrkdckgi.h"
#include "cardgi.h"


class GameGI : public QObject
{
    Q_OBJECT

public:
    explicit GameGI(QWidget * parent, MainWidget * form);
    ~GameGI();

    /**
     * Methods to repaint cards and their location on the board
     */
    void Rendering();

private:
    //! Member, which contains pointer to SElECTED card in GUI
    CardGI * FirstCard = nullptr;

    //! Pointer to MainWidget, is it because MainWidget contains public methods to create new game or delete this game
    MainWidget * bigform;
    //! Pointer to GameCore -> more in GameCore.h
    GameCore * game;
    //! Pointer to widget in which game have to be printed (it is Qlabel)
    QWidget * win;

    //! Main vertical layout of the win Widget
    QVBoxLayout * MainVBL;
    //! The top tabel of the game (it contains cell for Pack, Stacker, Buttons, and Targets)
    QGridLayout * Slots;
    //! Label, which write current status of game
    QLabel * LbStatus;
    //! Tabel of the Working stacks
    QGridLayout * Wrk_slot;
    //! Vertical box for buttons placed in Slots (column 2)
    QVBoxLayout * VbL;
    //! Button for New Game
    QPushButton * BtnNew;
    //! Button for End this game
    QPushButton * BtnEnd;
    //! Button for print Help
    QPushButton * BtnHelp;
    //! Button for Undo
    QPushButton * BtnUndo;
    //!Horizontal layout for buttons Save and Load
    QHBoxLayout * Hbl;
    //! Button for save game
    QPushButton * BtnSave;
    //! Button for load game
    QPushButton * BtnLoad;
    //! Card on the top of Pack
    CardGI * Pack = nullptr;
    //! Card on the top of Stacker
    CardGI * Stacker = nullptr;
    //! card on the top of Target of hearts
    CardGI * TargetH = nullptr;
    //! card on the top of Target of diamonds
    CardGI * TargetD = nullptr;
    //! card on the top of Target of clubs
    CardGI * TargetC = nullptr;
    //! card on the top of Target of spades
    CardGI * TargetS = nullptr;

    //! Cards on working stack 1
    WrkDckGI * W1;
    //! Cards on working stack 2
    WrkDckGI * W2;
    //! Cards on working stack 3
    WrkDckGI * W3;
    //! Cards on working stack 4
    WrkDckGI * W4;
    //! Cards on working stack 5
    WrkDckGI * W5;
    //! Cards on working stack 6
    WrkDckGI * W6;
    //! Cards on working stack 7
    WrkDckGI * W7;

    /**
     * Methods for finding on which warking stack is the card
     * @param str represent internal registration f stacks
     * @return pointer to Working stacks in GameCore
     */
    Working* whichWorking(QString str);
    /**
     * Methods for finding on which target is meaning
     * @param str represent internal registration f stacks
     * @return pointer to Target stacks in GameCore
     */
    Target* whichTarget(QString str);
    /**
     * Methods for finding where card have its target
     * @param card
     * @return pointer to Target stacks in GameCore
     */
    Target* getTarget(KCard * card);

public slots:
    /**
     * Methods which is invoke, when Pack is clicked
     * @param ClkdCard
     */
    void PackClick(CardGI * ClkdCard);
    /**
     * Methods which is invoke, when other card stack is clicked
     * @param ClkdCard
     */
    void DeckClick(CardGI * ClkdCard);
    /**
     * Methods for double click to one card
     * @param ClkdCard
     */
    void DoublDecClick(CardGI *ClkdCard);

    /**
     * When is clicked on BtnNew
     */
    void NewClick();
    /**
     * When is clicked on BtnEnd
     */
    void EndClick();

    /**
     * When is BtnHelp clicked
     */
    void HelpClick();
    /**
     * When is BtnUndo clicked
     */
    void UndoClick();
    /**
     * When is BtnSave clicked
     */
    void SaveClick();
    /**
     * When is BtnLoad clicked
     */
    void LoadClick();
};

#endif // GAMEGI_H
