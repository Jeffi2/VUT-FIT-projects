/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class for form in game
 *
 */

#include "mainwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <iostream>

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    this->resize(1600,900);
    this->setWindowIcon(QIcon("./images/icons/64x64/solitaire.png"));

    gameTab = new QGridLayout(this);
    gameTab->setContentsMargins(0,0,0,0);
    gameTab->setSpacing(0);

    for(int i = 0; i < 4; i++)
    {
        LbBack[i] = new QLabel("");

        if(i == 0 || i == 3)
            LbBack[i]->setStyleSheet("QLabel { background-color : #004F0F; color: white; }");
        else
            LbBack[i]->setStyleSheet("QLabel { background-color : #004A0E; color: white; }");

        LbBack[i]->setContentsMargins(0,0,0,0);

        LbBack[i]->setVisible(false);

        int row = i/2;
        int column = i%2;

        gameTab->addWidget(LbBack[i],row,column);
        Games[i] = nullptr;
    }

    Games[0] = new GameGI(LbBack[0],this);
    renderGi();
}

MainWidget::~MainWidget()
{
    delete gameTab;



    for(int i = 0; i < 4; i++)
    {

        if(Games[i] != nullptr)
            delete Games[i];

        delete LbBack[i];


    }
}

void MainWidget::renderGi()
{
    int i = howManyG();

    if(i == 0)
    {
        qApp->quit();
        return;
    }

    if(i == 1)
    {
        int i = 0;
        for(i = 0; i < 4; i++)
        {
            if(Games[i] != nullptr)
                break;
        }

        visibleOnly(i);
        return;
    }
    else
    {
        visibleAll();
        return;
    }
}

int MainWidget::howManyG()
{
    int ret = 0;

    for(int i = 0; i < 4 ; i++)
    {
        if(Games[i] != nullptr)
            ret++;
    }

    return ret;
}

void MainWidget::visibleAll()
{
    for(int i = 0; i< 4; i++)
    {
        LbBack[i]->setVisible(true);
    }
}

void MainWidget::visibleOnly(int index)
{
    for(int i = 0; i< 4; i++)
    {
        LbBack[i]->setVisible(false);
    }

    LbBack[index]->setVisible(true);
}

void MainWidget::NewGClick()
{
    int num_games = howManyG();

    if(num_games > 3)
        return;

    int index;

    for(index = 0; index < 4;index++)
    {
        if(Games[index] == nullptr)
            break;
    }

    Games[index] = new GameGI(LbBack[index],this);
    renderGi();
}

void MainWidget::EndGClick(GameGI *curGame)
{
    int i;

    for(i = 0; i < 4; i++)
    {
        if(Games[i] == curGame)
            break;
    }

    delete Games[i];
    Games[i] = nullptr;

    renderGi();
}

