/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing stack of cards.
 *
 */

#include "CardStack.h"

void CardStack::add(KCard* card) {
    CardList.insert(CardList.begin(), card);
}

void CardStack::remove(){
    CardList.erase(CardList.begin());
}

bool CardStack::put(CardStack* stack){
    if (!stack->isEmpty()) {
        for (int i = stack->size(); i > 0 ; --i) {
            int goodIndex = i - 1;
            CardList.insert(CardList.begin(), stack->get(goodIndex));
        }

        for (int j = stack->size(); j > 0 ; --j) {
            stack->remove();
        }
        return true;
    }
    return false;
}

CardStack* CardStack::pop(KCard* card){
    int index = -1;
    bool isFound = false;
    for (int i = 0; i < CardList.size(); ++i) {
        if (CardList[i]->color() == card->color() && CardList[i]->value() == card->value()) {
            if (isFound) {
                break;
            }
            index = i;
            isFound = true;
        }
        else {
            if (isFound) break;
        }
    }

    if (index == -1) {
        return nullptr;
    }
    CardStack* removedCards = new CardStack();
    if (index == 0) {
        removedCards->add(CardList[index]);
        CardList.erase(CardList.begin());
    }
    else {
        for (int i = 0; i < index+1; i++) {
            removedCards->add(CardList[i]);
           // CardList.erase(CardList.begin()); //TODO debug
        }
        for (int j = 0; j < index+1; ++j) {
            CardList.erase(CardList.begin());
        }
    }
    return removedCards;
}

bool CardStack::superput(CardStack* stack){
    //should not call here only in inheritance
    return false; //this shoud not be return!!!
}

bool CardStack::moveCards(CardStack* source, KCard* card, CardStack* dest){
    CardStackCMD* cmd = new CardStackCMD(this, source, card, dest);
    return invoker->execute(cmd);
}

CardStack::CardStackCMD::CardStackCMD(CardStack *obj, CardStack *source, KCard *card, CardStack *dest) {
    this->stack = obj;
    this->source = source;
    this->card = card;
    this->dest = dest;
    this->turnedFaceUp = false;
}

bool CardStack::CardStackCMD::execute() {
    if (!this->source->isEmpty()) {
        if (this->card->isTurnedFaceUp()) {
            CardStack *temp = source->pop(this->card);
            if (this->dest->put(temp)){
                stack->getUndoStack()->put(stack->getInvoker());
                if (!source->isEmpty()){
                    KCard* top = source->get();
                    if (!top->isTurnedFaceUp()){
                        turnedFaceUp = true;
                        top->turnFaceUp();
                    }
                }
                temp->deletePonters();
                delete temp;
                return true;
            } else {
                source->superput(temp); //TODO check if i need a super put???
            }
            temp->deletePonters();
            delete temp;
        }
    }
    return false;
}

void CardStack::CardStackCMD::undo() {
    if (this->turnedFaceUp) {
        KCard* temp = source->get();
        temp->turnFaceDown();
    }
    CardStack* temp = dest->pop(this->card);
    source->superput(temp); // TODO super put???
    delete temp;
}