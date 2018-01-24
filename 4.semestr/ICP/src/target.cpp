/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing target stack for cards
 *
 */

#include "target.h"

Target::Target(CardColor::Color color, GlobalUndo* undo, std::string stack){
    this->packColor = color;
    this->globalUndo = undo;
    this->nam = "T"+stack;
}


bool Target::put(KCard* card) {
    if (!card->isTurnedFaceUp()) return false;
    if (CardList.empty()) {
        if (card->value() == 1 && card->color() == packColor) {
            CardList.insert(CardList.begin(), card);
            return true;
        }
    }
    else {
        if (card->color() == packColor && CardList[0]->value()+1 == card->value()) {
            CardList.insert(CardList.begin(), card);
            return true;
        }
    }
    return false;
}

bool Target::moveCard(CardDeck* source, CardDeck* dest) {
    TargetStackCMD* cmd = new TargetStackCMD(this, source, dest);
    return invoker->execute(cmd);
}

bool Target::next(KCard* card) {
    if (CardList.empty()) {
        if (card->value() == 1 && card->color() == this->packColor) {
            return true;
        }
    } else {
        if (card->color() == packColor && CardList[0]->value()+1 == card->value()) {
            return true;
        }
    }
    return false;
}

Target::TargetStackCMD::TargetStackCMD(Target *obj, CardDeck *src, CardDeck *dest) {
    stack = obj;
    source = src;
    this->dest = dest;
    this->turnedDown = false;
}

bool Target::TargetStackCMD::execute() {
    if (!source->isEmpty()){
        KCard* temp = source->get();
        if (temp->isTurnedFaceUp()){
            if (dest->put(temp)){
                stack->getUndoStack()->put(stack->getInvoker());
                source->pop();
                if (source->size() > 0 ){
                    temp = source->get();
                    if (!temp->isTurnedFaceUp()) {
                        turnedDown = true;
                        temp->turnFaceUp();
                    }
                }
                return true;
            }
        }
    }
    return false;
}

void Target::TargetStackCMD::undo() {
    if (turnedDown) {
        KCard* temp = source->get();
        temp->turnFaceDown();
    }
    KCard* temp = dest->pop();
    source->superput(temp);
}