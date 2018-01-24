/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing pack of cards.
 *
 */

#include "CardDeck.hpp"
#include "card.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <fstream>
#include <cstdlib>

void CardDeck::suffle() {
    int index;
    KCard* temp;
    for (int i = 0; i < CardList.size(); i++){
        index = (int) (rand() %(CardList.size() - i)) + i;
        temp = CardList[i];
        CardList[i] = CardList[index];
        CardList[index] = temp;
    }
}

KCard* CardDeck::pop()
{
    if (!CardList.empty()) {
        KCard *temp = CardList.front();
        CardList.erase(CardList.begin());
        return temp;
    }
    return nullptr;
}

KCard* CardDeck::get()
{
    if (CardList.empty()) {
        return nullptr;
    }
    return CardList[0];
}

KCard* CardDeck::get(int index)
//Kde se to sakra pouzi? proc to revertuju? :D
{
    if (index < 0 || index > (CardList.size()-1)) {
        return nullptr;
    }
    int reCount = ((int)(CardList.size()-1) - index);
    return CardList[reCount];
}

bool CardDeck::isEmpty()
{
    return CardList.empty();
}

bool CardDeck::put(KCard* card)
{
    CardList.insert(CardList.begin(), card);
    return true;
}

bool CardDeck::putToEnd(KCard* card)
{
    CardList.insert(CardList.end(), card);
    return true;
}

int CardDeck::size()
{
    return (int)CardList.size();
}

CardDeck::~CardDeck()
{
    for (int i = 0; i < (int)CardList.size(); i++) {
        delete (CardList[i]);
    }
    delete invoker;
    //std::cout << "Card deck should be free." <<std::endl;
}

void CardDeck::print()
{
    for (int i=0; i < (int)CardList.size(); i++) {
        std::string face;
        KCard *tmp = CardList[i];
        if (tmp->isTurnedFaceUp()){
            face = "Vis";
        }
        else {
            face = "UNVis";
        }
        std::string barva;
        int int_color = tmp->color();
        switch (int_color) {
            case 0:
                barva = "C";
                break;
            case 1:
                barva = "S";
                break;
            case 2:
                barva = "H";
                break;
            case 3:
                barva = "D";
                break;
            default:
                break;
        }
        std::cout << tmp->value() << barva << " "<< face << "   ";
        std::cout.flush();
    }
}


bool CardDeck::save(std::ofstream& file){
    //TODO handle exception and return false
    std::string sizeOfstack = std::to_string((int)CardList.size());
    sizeOfstack += "\n";
    file.write(sizeOfstack.data(), sizeOfstack.size());

    for (int i = 0; i < CardList.size(); ++i) {
        KCard* card = CardList[i];
        std::string writeStr = std::to_string(card->value()) ;
        writeStr += " "+std::to_string(card->color())+" "+std::to_string(card->isTurnedFaceUp())+"\n";
        file.write(writeStr.data(), writeStr.size());
    }
    return true;
}

bool CardDeck::load(std::ifstream& file){
    std::string convert;
    getline(file, convert);
    int count;
    try {
        count = std::stoi(convert);
    }
    catch (...){
        return false;
    }
    std::string color;
    std::string value;
    std::string face;

    for (int i = 0; i < count; ++i) {
        getline(file, convert);
        if (convert.find(" ") == std::string::npos) {
            return false;
        }
        int spaceindex = (int)convert.find(" ");
        value = convert.substr(0, spaceindex);
        convert = convert.substr(spaceindex+1);
        color = convert[0];
        face = convert[2];
        CardColor::Color col;
        int colint;
        try {
            colint = std::stoi(color);
        }
        catch (...){
            return false;
        }
        switch (colint) {
            case 0:
                col = CardColor::Color::clubs;
                break;
            case 1:
                col = CardColor::Color::spides;
                break;
            case 2:
                col = CardColor::Color::hearts;
                break;
            case 3:
                col = CardColor::Color::diamonds;
                break;
            default:
                return false;
        }
        KCard* card = new KCard(col, std::stoi(value), std::stoi(face));
        this->putToEnd(card);
        convert = "";
        face = "";
        value = "";
        color = "";
    }

    return true;
}

std::string CardDeck::str_card(int index)
{
    if (CardList.size() == 0)
    {
        if (index == -1 || index == 0)
            return "[><]";
        else
            return "";
    }

    if(index < 0)
        index = (int)CardList.size()-1;

    index = (int)CardList.size() - index - 1;
    if (index < 0)
        return "";

    KCard *tmp = CardList[index];
    if (!tmp->isTurnedFaceUp())
        return "[]";

    std::string card="";

    switch (tmp->value())
    {
        case 1:
            card="A";
            break;
        case 11:
            card="J";
            break;
        case 12:
            card="Q";
            break;
        case 13:
            card="K";
            break;
        default:
            card=std::to_string(tmp->value());
            break;
    }

    int int_color = tmp->color();
    switch (int_color) {
        case 0:
            card += "C";
            break;
        case 1:
            card += "S";
            break;
        case 2:
            card += "H";
            break;
        case 3:
            card += "D";
            break;
        default:
            break;
    }

    return card;
}

std::string CardDeck::str_LastCard()
{
    return str_card(-1);
}


bool Invoker::execute(Command* cmd){
    bool tester = cmd->execute();
    if (tester) {
        list.insert(list.begin(), cmd);
        return true;
    }
    else {
        delete cmd;
        return false;
    }
}

void Invoker::undo() {
    if (list.size() > 0){
        Command* tmp = list.front();
        tmp->undo();
        delete tmp;
        list.erase(list.begin());
    }
}

Invoker::~Invoker() {
    for (int i = 0; i < (int) list.size(); i++) {
        delete (list[i]);
    }
}

void GlobalUndo::put(Invoker* invoker){
    undoStack.insert(undoStack.begin(), invoker);
}

void GlobalUndo::makeUndo(){
    if (undoStack.size() > 0) {
        undoStack[0]->undo();
        undoStack.erase(undoStack.begin());
    }
}

void GlobalUndo::removePointers(){
    undoStack.clear();
}

bool CardDeck::next(KCard* card){
    return false;
}

void CardDeck::deletePonters(){
    CardList.clear();
}

CardDeck::CardDeck() {
    invoker = new Invoker();
    this->nam = "Stacker";
}

CardDeck::CardDeck(GlobalUndo* undo){
    invoker = new Invoker();
    this->globalUndo = undo;
    this->nam = "Stacker";
}

KCard* CardDeck::getRealIn(int index){
    return CardList[index];
}

bool CardDeck::superput(KCard* card){
    //Call only in inheritance
    put(card);
}

CardDeck* CardDeck::help(std::vector<CardDeck*> *list){
    //This signalize problem :(
    for (int i = 0; i < list->size(); i++) {
        CardDeck* item = list->at(i);
        if (CardList.empty()) {
            return nullptr;
        }
        if (item->next(this->get())){
            return item;
        }
    }
    return nullptr;
}

std::string CardDeck::name(){
    return this->nam;
}

Invoker* CardDeck::getInvoker() {
    return this->invoker;
}

GlobalUndo* CardDeck::getUndoStack(){
    return this->globalUndo;
}

bool CardDeck::moveCard(CardDeck* source ,CardDeck* dest){
    CardDeckCMD* cmd = new CardDeck::CardDeckCMD(this, source, dest);
    globalUndo->put(invoker);
    return invoker->execute(cmd);
}

CardDeck::CardDeckCMD::CardDeckCMD(CardDeck* obj, CardDeck* source, CardDeck* dest) {
    this->stack = obj;
    this->source = source;
    this->dest = dest;
}

bool CardDeck::CardDeckCMD::execute() {
    if (!source->isEmpty()) {
        KCard* temp = source->pop();
        temp->turnFaceUp();
        dest->put(temp);
    } else {
        int stackerSize = dest->size();
        for (int i = 0; i < stackerSize; ++i) {
            KCard* temp = dest->pop();
            temp->turnFaceDown();
            source->put(temp);
        }
    }
    return true;
}

void CardDeck::CardDeckCMD::undo() {
    if (!dest->isEmpty()) {
        KCard* tmp = dest->pop();
        tmp->turnFaceDown();
        source->put(tmp);
    } else {
        int stackerSize = source->size();
        for (int i = 0; i < stackerSize; ++i) {
            KCard* tmp = source->pop();
            tmp->turnFaceUp();
            dest->put(tmp);
        }
    }
}