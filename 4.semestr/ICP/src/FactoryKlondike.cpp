/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Factory for creating card, stacks, ...
 *
 */


#include "FactoryKlondike.hpp"
#include "working.h"
#include <iostream>
#include <cstddef>


KCard* FactoryKlondike::createCard(CardColor::Color color, int value){
    if (value > 0 && value <= 13)
        if (color == CardColor::Color::clubs || color == CardColor::Color::spides || color == CardColor::Color::hearts || color == CardColor::Color::diamonds) {
            
            return new KCard(color, value);
        }
    return nullptr;
}

CardDeck* FactoryKlondike::createEmptyDeck(GlobalUndo* undo) {
    CardDeck* empty = new CardDeck(undo);
    return empty;
}

CardDeck* FactoryKlondike::createCardDeck(GlobalUndo* undo)
{
    CardDeck* deck = new CardDeck(undo);
    for (int i = 1; i <= 13; i++) {
        deck->put(new KCard(CardColor::Color::clubs, i));
    }
    for (int i = 1; i <= 13; i++) {
        deck->put(new KCard(CardColor::Color::diamonds, i));
    }
    for (int i = 1; i <= 13; i++) {
        deck->put(new KCard(CardColor::Color::spides, i));
    }
    for (int i = 1; i <= 13; i++) {
        deck->put(new KCard(CardColor::Color::hearts, i));
    }
    return deck;
}

Working* FactoryKlondike::createWorkingPack(CardDeck* cardPack, int numerOfCards, GlobalUndo* undo)
{
    Working* createdStack = new Working(undo, numerOfCards);
    for (int i = 0; i < numerOfCards; ++i) {
        KCard* card = cardPack->pop();
        createdStack->superput(card);
    }
    createdStack->get()->turnFaceUp();
    CardDeck* tmp = createdStack;
    this->addItem(tmp);
    return createdStack;
}

Working* FactoryKlondike::createEmptyWorking(GlobalUndo* undo, int number) {
    Working* newWorking = new Working(undo, number);
    CardDeck* tmp = newWorking;
    this->addItem(tmp);
    return newWorking;
}

CardDeck* FactoryKlondike::createStacker(){
    CardDeck* createdStacker = new CardDeck();
    this->addItem(createdStacker);
    return createdStacker;
}

Target* FactoryKlondike::createTargetStack(CardColor::Color color, GlobalUndo* undo, std::string stack) {
    Target* createdTarget = new Target(color, undo, stack);
    CardDeck* tmp = createdTarget;
    this->addItem(tmp);
    return createdTarget;
}

void FactoryKlondike::addItem(CardDeck* item) {
    items->insert(items->end(), item);
}

std::vector<CardDeck*>* FactoryKlondike::getItems(){
    return items;
}

void FactoryKlondike::clearItems(){
    items->clear();
}

FactoryKlondike::~FactoryKlondike(){
    delete items;
}