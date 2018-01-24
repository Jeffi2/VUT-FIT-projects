/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing card.
 *
 */

#include "card.hpp"


KCard::KCard(CardColor::Color color, int value){
    val = value;
    col = color;
}

KCard::KCard(CardColor::Color color, int value, bool turn){
    val = value;
    col = color;
    face = turn;
}

CardColor::Color KCard::color() {
    return this->col;
}

int KCard::compareValue(KCard* c){
    return this->val - c->value();
}

bool KCard::isTurnedFaceUp(){
    return this->face;
}

bool KCard::similarColorTo(KCard* c){
    int cardColor = c->color();
    if ((this->col == 0 || this->col == 1) && (cardColor == 0 || cardColor == 1)) {
        return true;
    }
    if ((this->col == 2 || this->col == 3) && (cardColor == 2 || cardColor == 3)) {
        return true;
    }
    return false;
}

bool KCard::turnFaceUp(){
    if (this->face) {
        return false;
    }
    else {
        this->face = true;
        return true;
    }
}

bool KCard::turnFaceDown(){
    if (this->face) {
        this->face = false;
        return true;
    }
    else
    {
        return false;
    }
}

int KCard::value(){
    return this->val;
}

bool KCard::same_card(KCard *c)
{
    if(!this->face)
        return false;

    if(this->col != c->col)
        return false;

    if(this->val != c->val)
        return false;

    return true;
}
