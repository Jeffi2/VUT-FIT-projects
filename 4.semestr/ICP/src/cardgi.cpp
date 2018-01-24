/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing card in GUI (contains pixmap, and clicabel event)
 *
 */

#include "cardgi.h"
#include <QPixmap>

#include <iostream>

CardGI::CardGI(const QString &text, QWidget *parent, KCard *card)
{
    this->setFixedHeight(110);
    this->setFixedWidth(75);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setCursor(Qt::PointingHandCursor);

    CardStack = text;
    this->rewrite(card);
}

void CardGI::rewrite(KCard *card)
{
    QPixmap pix;

    if(card == nullptr)
    {
        card_loc = card;

        QString file = "./images/" + CardStack + ".png";
        pix.load(file);
    }
    else
    {
        //if(card == card_loc && (card->isTurnedFaceUp() == card_loc->isTurnedFaceUp()))
        //{
        //    return;
        //}

        card_loc = card;

        if(card->isTurnedFaceUp())
        {
            QString card_txt = "./images/card/" + QString::number(card->value());

            int int_color = card->color();
            switch (int_color)
            {
                case 0:
                    card_txt += "C";
                    break;
                case 1:
                    card_txt += "S";
                    break;
                case 2:
                    card_txt += "H";
                    break;
                case 3:
                    card_txt += "D";
                    break;
                default:
                    break;
            }

            QString file = card_txt + "-M.svg";
            pix.load(file);
        }
        else
        {
            pix.load("./images/back.svg");
        }
    }

    this->setPixmap(pix);
}

QString CardGI::getCardName()
{
    KCard *tmp = this->card_loc;
    if (!tmp->isTurnedFaceUp())
        return "";

    QString card_name = "";

    switch (tmp->value())
    {
        case 1:
            card_name="Ace";
            break;
        case 11:
            card_name="Jack";
            break;
        case 12:
            card_name="Qeen";
            break;
        case 13:
            card_name="King";
            break;
        default:
            card_name=QString::number(tmp->value());
            break;
    }

    card_name += " ";

    int int_color = tmp->color();
    switch (int_color) {
        case 0:
            card_name += "Clubs";
            break;
        case 1:
            card_name += "Spades";
            break;
        case 2:
            card_name += "Hearts";
            break;
        case 3:
            card_name += "Diamonds";
            break;
        default:
            break;
    }

    return card_name;

}

CardGI::~CardGI() {}



void CardGI::mousePressEvent ( QMouseEvent * event )
{
    emit clicked(this);
}




