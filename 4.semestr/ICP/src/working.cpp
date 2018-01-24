/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing working stack for cards
 *
 */

#include "working.h"

bool Working::put(KCard* card){
    if (!card->isTurnedFaceUp()) return false;
    if (CardList.empty()){
        if (card->value() == 13) {
            CardList.insert(CardList.begin(), card);
            return true;
        }
    } else {
        if (!card->similarColorTo(CardList[0]) && CardList[0]->value()-1 == card->value() ) {
            CardList.insert(CardList.begin(), card);
            return true;
        }
    }
    return false;
}

bool Working::superput(KCard* card){
    CardDeck::put(card);
}

bool Working::put(CardStack* stack){
    if (stack->size() == 0) {
        return false;
    }
    for (int i = 0; i < stack->size() ; ++i) {
        //there is revert get?
        if (!stack->get(0)->isTurnedFaceUp()) return false;
    }
    if ((CardList.empty() && (stack->get((stack->size()-1))->value() == 13))
        || (!CardList.empty() && (stack->get(stack->size()-1)->value()+1) == CardList[0]->value() && !stack->get(stack->size()-1)->similarColorTo(CardList[0]))) {

        if (stack->size() >= 2) {
            for (int i = 0; i < stack->size()-1; ++i) {
                if (stack->get(i)->similarColorTo(stack->get(i+1)) || stack->get(i)->value()+1 != stack->get(i+1)->value() ) {
                    return false;
                }
            }
        }
        for (int j = stack->size(); j > 0 ; --j) {
            int goodIndex = j-1;
            CardList.insert(CardList.begin(), stack->get(goodIndex));
        }/*
        for (int k = stack->size(); k > 0 ; --k) {
            CardList.erase(CardList.begin());
         }*/
        return true;
    }
    return false;
}

bool Working::superput(CardStack* stack){
    CardStack::put(stack);
}

Working::Working(GlobalUndo* undo, int count) {
    globalUndo = undo;
    this->nam="W"+std::to_string(count);
}

bool Working::moveCard(CardDeck* source ,CardDeck* dest) {
    WorkingCMD* cmd = new WorkingCMD(this, source, dest);
    return invoker->execute(cmd);
}

bool Working::next(KCard* card) {
    if (CardList.empty()) {
        if (card->value() == 13) {
            return true;
        }
    }
    else {
        if (!card->similarColorTo(CardList[0]) && CardList[0]->value()-1 == card->value()) {
            return true;
        }
    }
    return false;
}

CardDeck* Working::help(std::vector<CardDeck*> *list) {
    for (int i = 0; i < list->size(); ++i) {
        CardDeck* item = list->at(i);
        if (this->isEmpty()) {
            return nullptr;
        }
        for (int j = 0; j < this->size(); ++j) {
            if (!this->getRealIn(j)->isTurnedFaceUp()){
                break;
            }
            if (item->next(this->getRealIn(j))) {
                return item;
            }
        }
    }
    return nullptr;
}

Working::WorkingCMD::WorkingCMD(Working* obj, CardDeck* src, CardDeck* dest ){
    this->stack = obj;
    this->source = src;
    this->dest = dest;
    turnedDown = false;
}

bool Working::WorkingCMD::execute() {
    if (!source->isEmpty()){
        KCard* temp = source->get();
        if (temp->isTurnedFaceUp()){
            if (dest->put(temp)){
                stack->getUndoStack()->put(stack->getInvoker());
                source->pop();
                if (source->size() > 0){
                    temp = source->get();
                    if (!temp->isTurnedFaceUp()){
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

void Working::WorkingCMD::undo() {
    if (turnedDown) {
        KCard* temp = source->get();
        temp->turnFaceDown();
    }
    KCard* tmp = dest->pop();
    source->superput(tmp);
}