/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class for representing Working Stack in GUI
 *
 */

#ifndef WRKDCKGI_H
#define WRKDCKGI_H

#include <QWidget>

class WrkDckGI;

#include "gamegi.h"
#include "cardgi.h"

class WrkDckGI
{
public:
    explicit WrkDckGI(QWidget *parent = 0, GameGI *CurGame =0, Working* CurWrk = 0, const QString &text="", QGridLayout * Slots = 0,int row = 1, int colmn =0);
    ~WrkDckGI();

    /**
     * Rewrite card in working stacks
     * @param curWrk
     */
    void rewrite(Working * curWrk);

private:
    //! pointer to Game GUI
    GameGI *game;
    //! pointer to Backgorun label, which is created in MainWidgate
    QWidget *win;
    //! Pointer to working stack
    Working *wrk;
    //! String, which is charakterize working stack
    QString StrDef;

    //! Card Label, which is working stack empty
    CardGI * emptyCard;

    //! Pointer to Grid Layout, which is created in Game GUI whit name of Wrk_Slots
    QGridLayout * tabel;
    //! row is it this working stack print
    int i_row;
    //! column where is working stack print in the Grid Layout
    int i_colmn;

    //! Vector of Cards in Wrk stack
    QVector<CardGI*> cards;
    /**
     * Add Card into the card vector
     * @param index
     */
    void AppendCardGI(int index);
    /**
     * How many cards have to be eleted
     * @param num - number of deleted cards
     */
    void DeleteCardGI(int num);

};

#endif // WRKDCKGI_H
