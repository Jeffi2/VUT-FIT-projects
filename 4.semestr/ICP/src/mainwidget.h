/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class for form in game
 *
 */

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

class MainWidget;

#include "gamegi.h"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    /**
     * Catch clicked on BtnNew in running games
     */
    void NewGClick();
    /**
     * Catch clciked on BtnEnd in running game
     * @param curGame
     */
    void EndGClick(GameGI * curGame);

private:
    //! The mina tabel, which divided mainWifget into 2x2 tabel
    QGridLayout * gameTab;
    //! Bacground Label of the games
    QLabel * LbBack[4];
    //! GUI games
    GameGI * Games[4] = {nullptr};

    /**
     * Methods for render GUI games
     */
    void renderGi();
    /**
     * Methods for returning number of games
     * @return number of running games
     */
    int howManyG();
    /**
     * Methods for visible all background label, despite label is empty
     */
    void visibleAll();
    /**
     * Methods, which set visible only one running game
     * @param index of the running game
     */
    void visibleOnly(int index);
};

#endif // MAINWIDGET_H
