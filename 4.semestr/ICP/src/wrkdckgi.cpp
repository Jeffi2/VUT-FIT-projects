/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class for representing Working Stack in GUI
 *
 */

#include "wrkdckgi.h"

WrkDckGI::WrkDckGI(QWidget *parent, GameGI *CurGame, Working* CurWrk, const QString &text, QGridLayout * Slots,int row, int colmn)
{
    win = parent;
    game = CurGame;
    wrk = CurWrk;
    StrDef = text;

    tabel = Slots;
    i_row = row;
    i_colmn = colmn;

    emptyCard = new CardGI(StrDef+","+QString::number(-1),win,nullptr);
    tabel->addWidget(emptyCard,i_row,i_colmn,Qt::AlignCenter);
    QObject::connect(emptyCard, SIGNAL(clicked(CardGI*)), game, SLOT(DeckClick(CardGI*)));

    this->rewrite(wrk);

}


WrkDckGI::~WrkDckGI()
{
    DeleteCardGI(cards.size());
    delete emptyCard;
}



void WrkDckGI::rewrite(Working * curWrk)
{
    if(this->wrk != curWrk)
        wrk = curWrk;


    DeleteCardGI(cards.size()-wrk->size());

    int index = 0;

    while(index < wrk->size())
    {
        AppendCardGI(index);
        index++;
    }
}

void WrkDckGI::AppendCardGI(int index)
{
    if(index < cards.size())
    {
        cards.at(index)->rewrite(wrk->get(index));
    }
    else
    {
        CardGI * new_card = new CardGI(StrDef+","+QString::number(index),win,wrk->get(index));
        cards.push_back(new_card);

        QObject::connect(new_card, SIGNAL(clicked(CardGI*)), game, SLOT(DeckClick(CardGI*)));       
    }

    tabel->addWidget(cards.at(index),i_row+index,i_colmn,Qt::AlignCenter);
}

void WrkDckGI::DeleteCardGI(int num)
{
    int i;

    for(i = 0; i < num; i++)
    {
        delete cards.back();
        cards.pop_back();
    }
}
