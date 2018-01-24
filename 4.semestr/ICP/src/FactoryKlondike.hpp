/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Factory for creating card, stacks, ...
 *
 */
#ifndef FactoryKlondike_hpp
#define FactoryKlondike_hpp

#include "card.hpp"
#include <stdio.h>
#include "CardDeck.hpp"
#include "CardStack.h"
#include "working.h"
#include "target.h"

class FactoryKlondike
{
private:
   //! List of all created stacks and decks except Pack
   std::vector<CardDeck*> *items = new std::vector<CardDeck*>;
public:
    /**
     * Create one card
     * @param color Cards color
     * @param value Cards value
     * @return Pointer to card
     */
    KCard* createCard(CardColor::Color color, int value);
    /**
     * Create and fill Pack of 52 cards
     * @param undo Pointer to undo stack
     * @return Pointer to created card Pack
     */
    CardDeck* createCardDeck(GlobalUndo* undo);
    /**
     * Create empty Pack (for load)
     * @param undo Pointer to undo stack
     * @return Pointer to created card Pack
     */
    CardDeck* createEmptyDeck(GlobalUndo* undo);
    /**
     * Create Working stacks and fill cards from Pack
     * @param cardDeck Pack of cards
     * @param numerOfCards Count of cards to fill
     * @param undo Pointer to undo stack
     * @return Pointer to working stack
     */
    Working* createWorkingPack(CardDeck* cardDeck, int numerOfCards, GlobalUndo* undo);
    /**
     * Create empty Working stacks (for load)
     * @param undo Pointer to undo stack
     * @return Pointer to working stack
     */
    Working* createEmptyWorking(GlobalUndo* undo, int number);
    /**
     * Create stacker
     * @return Pointer to stacker
     */
    CardDeck* createStacker();
    /**
     * Create target stack od some color
     * @param color Color of target stack
     * @param undo Pointer to undo stack
     * @param stack Name for target stack
     * @return Pointer to created target stack
     */
    Target* createTargetStack(CardColor::Color color, GlobalUndo* undo, std::string stack);
    /**
     * Add item to items vector
     * @param item Stacker or Target or Working stack
     */
    void addItem(CardDeck* item);
    /**
     * Getter for items vector
     * @return Pointer to items
     */
    std::vector<CardDeck*>* getItems();
    /**
     * Delete all pointer in items
     */
    void clearItems();
    /**
     * Delete dynamic items
     */
    ~FactoryKlondike();
};

#endif /* card_hpp */
