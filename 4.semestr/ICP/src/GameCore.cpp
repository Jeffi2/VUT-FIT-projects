/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing game core of single game
 *
 */

#include <stdlib.h>
#include <sys/ioctl.h>
#include <zconf.h>
#include "GameCore.h"
#include <ctime>

GameCore::GameCore() {
    std::srand(unsigned(std::time(NULL)));
    factory = new FactoryKlondike();
    globalUndo = new GlobalUndo();
    standDeck = factory->createCardDeck(globalUndo);
    standDeck->suffle();
    stacker = factory->createStacker();
    targetC = factory->createTargetStack(CardColor::clubs, globalUndo, "C");
    targetD = factory->createTargetStack(CardColor::diamonds, globalUndo, "D");
    targetH = factory->createTargetStack(CardColor::hearts, globalUndo, "H");
    targetS = factory->createTargetStack(CardColor::spides, globalUndo, "S");
    working1 = factory->createWorkingPack(standDeck, 1, globalUndo);
    working2 = factory->createWorkingPack(standDeck, 2, globalUndo);
    working3 = factory->createWorkingPack(standDeck, 3, globalUndo);
    working4 = factory->createWorkingPack(standDeck, 4, globalUndo);
    working5 = factory->createWorkingPack(standDeck, 5, globalUndo);
    working6 = factory->createWorkingPack(standDeck, 6, globalUndo);
    working7 = factory->createWorkingPack(standDeck, 7, globalUndo);

}

void GameCore::printAll() {
    std::cout << "PRINT START: "<<std::endl;
    std::cout << "Pack: "; standDeck->print();
    std::cout << std::endl;
    std::cout << "Stacker: ";stacker->print();
    std::cout << std::endl;
    std::cout << "W1: ";working1->print();
    std::cout << std::endl;
    std::cout << "W2: ";working2->print();
    std::cout << std::endl;
    std::cout << "W3: ";working3->print();
    std::cout << std::endl;
    std::cout << "W4: ";working4->print();
    std::cout << std::endl;
    std::cout << "W5: ";working5->print();
    std::cout << std::endl;
    std::cout << "W6: ";working6->print();
    std::cout << std::endl;
    std::cout << "W7: ";working7->print();
    std::cout << std::endl;
    std::cout << "TC: ";targetC->print();
    std::cout << std::endl;
    std::cout << "TH: ";targetH->print();
    std::cout << std::endl;
    std::cout << "TS: ";targetS->print();
    std::cout << std::endl;
    std::cout << "TD: ";targetD->print();
    std::cout << "PRINT END" << std::endl;
}

std::string strPad(std::string input)
{                        //012345
    std::string out_str = "      ";
    if(input.size() == 4)
    {
        out_str.replace(1,4,input);
    }
    else if (input.size() == 2)
    {
        out_str.replace(2,2,input);
    }
    else if(input.size() == 3)
    {
        out_str.replace(1,3,input);
    }

    return out_str;
}

std::string full_line()
{
    return "------------------------------------------\n";
}



void GameCore::print_cli()
{
    //struct winsize w;
    //ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    system("tput clear");
    system("cat banner.txt");

    std::string line;

    line = full_line()+strPad("Pack")+strPad("")+strPad("")+strPad("TrgH")+strPad("TrgD")+strPad("TrgC")+strPad("TrgS")+"\n";
    line += strPad(standDeck->str_LastCard())+
            strPad(stacker->str_LastCard())+
            strPad("")+
            strPad(targetH->str_LastCard())+
            strPad(targetD->str_LastCard())+
            strPad(targetC->str_LastCard())+
            strPad(targetS->str_LastCard())+
            "\n";

    line += full_line()+"\n";
    line += strPad("Wrk1")+strPad("Wrk2")+strPad("Wrk3")+strPad("Wrk4")+strPad("Wrk5")+strPad("Wrk6")+strPad("Wrk7")+"\n";

    std::cout <<line;

    for (int i = 0; i < max_Wstack(); ++i)
    {
        line =  strPad(working1->str_card(i))+
                strPad(working2->str_card(i))+
                strPad(working3->str_card(i))+
                strPad(working4->str_card(i))+
                strPad(working5->str_card(i))+
                strPad(working6->str_card(i))+
                strPad(working7->str_card(i))+
               "\n";

        std::cout <<line;
    }

    std::cout << full_line();
    std::cout << "|   E for EXIT game  |  H for more HELP  |\n";
    std::cout << full_line();
    std::cout << "cmd: ";
}

bool GameCore::saveGame(std::string path) {
    std::ofstream file;
    file.open(path);
    if (!file.is_open()) {
        return false;
    }
    standDeck->save(file);
    stacker->save(file);
    working1->save(file);
    working2->save(file);
    working3->save(file);
    working4->save(file);
    working5->save(file);
    working6->save(file);
    working7->save(file);
    targetC->save(file);
    targetH->save(file);
    targetS->save(file);
    targetD->save(file);
    file.close();
    return true;
}

bool GameCore::winGame(){
    if (this->getTargetD()->size()+this->getTargetC()->size()+
        this->getTargetH()->size()+this->getTargeS()->size() == 52) {
        return true;
    }
    return false;
}

bool GameCore::loadGame(std::string path) {
    std::ifstream file;
    file.open(path);
    if (!file.is_open())
        return false;
    bool test = true;
    CardDeck* standDeckT = factory->createEmptyDeck(globalUndo);
    CardDeck* stackerT = factory->createStacker();
    Target* targetCT = factory->createTargetStack(CardColor::clubs, globalUndo, "C");
    Target* targetDT = factory->createTargetStack(CardColor::diamonds, globalUndo, "D");
    Target* targetHT = factory->createTargetStack(CardColor::hearts, globalUndo, "H");
    Target* targetST = factory->createTargetStack(CardColor::spides, globalUndo, "S");
    Working* working1T = factory->createEmptyWorking(globalUndo, 1);
    Working* working2T = factory->createEmptyWorking(globalUndo, 2);
    Working* working3T = factory->createEmptyWorking(globalUndo, 3);
    Working* working4T = factory->createEmptyWorking(globalUndo, 4);
    Working* working5T = factory->createEmptyWorking(globalUndo, 5);
    Working* working6T = factory->createEmptyWorking(globalUndo, 6);
    Working* working7T = factory->createEmptyWorking(globalUndo, 7);

    if (!standDeckT->load(file)) test = false;
    if (!stackerT->load(file)) test = false;
    if (!working1T->load(file)) test = false;
    if (!working2T->load(file)) test = false;
    if (!working3T->load(file)) test = false;
    if (!working4T->load(file)) test = false;
    if (!working5T->load(file)) test = false;
    if (!working6T->load(file)) test = false;
    if (!working7T->load(file)) test = false;
    if (!targetCT->load(file)) test = false;
    if (!targetHT->load(file)) test = false;
    if (!targetST->load(file)) test = false;
    if (!targetDT->load(file)) test = false;

    delete stackerT;
    delete standDeckT;
    delete working1T;
    delete working2T;
    delete working3T;
    delete working4T;
    delete working5T;
    delete working6T;
    delete working7T;
    delete targetST;
    delete targetHT;
    delete targetDT;
    delete targetCT;
    if (!test)
        return false;

    delete stacker;
    delete standDeck;
    delete working1;
    delete working2;
    delete working3;
    delete working4;
    delete working5;
    delete working6;
    delete working7;
    delete targetS;
    delete targetH;
    delete targetD;
    delete targetC;
    globalUndo->removePointers();
    factory->clearItems();

    standDeck = factory->createEmptyDeck(globalUndo);
    stacker = factory->createStacker();
    targetC = factory->createTargetStack(CardColor::clubs, globalUndo, "C");
    targetD = factory->createTargetStack(CardColor::diamonds, globalUndo, "D");
    targetH = factory->createTargetStack(CardColor::hearts, globalUndo, "H");
    targetS = factory->createTargetStack(CardColor::spides, globalUndo, "S");
    working1 = factory->createEmptyWorking(globalUndo, 1);
    working2 = factory->createEmptyWorking(globalUndo, 2);
    working3 = factory->createEmptyWorking(globalUndo, 3);
    working4 = factory->createEmptyWorking(globalUndo, 4);
    working5 = factory->createEmptyWorking(globalUndo, 5);
    working6 = factory->createEmptyWorking(globalUndo, 6);
    working7 = factory->createEmptyWorking(globalUndo, 7);
    file.seekg(0, std::ios::beg);
    standDeck->load(file);
    stacker->load(file);
    working1->load(file);
    working2->load(file);
    working3->load(file);
    working4->load(file);
    working5->load(file);
    working6->load(file);
    working7->load(file);
    targetC->load(file);
    targetH->load(file);
    targetS->load(file);
    targetD->load(file);

    return true;
}

GameCore::~GameCore() {
    delete stacker;
    delete factory;
    delete standDeck;
    delete working1;
    delete working2;
    delete working3;
    delete working4;
    delete working5;
    delete working6;
    delete working7;
    delete targetS;
    delete targetH;
    delete targetD;
    delete targetC;
    delete globalUndo;
}

int GameCore::max_Wstack()
{
    int array_of_workSize[7] = {0};

    array_of_workSize[0] = working1->size();
    array_of_workSize[1] = working2->size();
    array_of_workSize[2] = working3->size();
    array_of_workSize[3] = working4->size();
    array_of_workSize[4] = working5->size();
    array_of_workSize[5] = working6->size();
    array_of_workSize[6] = working7->size();

    return *std::max_element(array_of_workSize, array_of_workSize+7);
}

std::string GameCore::getHelp() {
    std::vector<CardDeck*>* items = this->factory->getItems();
    for (int i = 0; i < items->size(); i++) {
        CardDeck* tmp = items->at(i);
        CardDeck* item = tmp->help(items);
        if(item) {
            return "From: "+tmp->name()+"  To: "+item->name();
        }
    }
    if (this->standDeck->isEmpty()){
        if (this->stacker->isEmpty()) {
            return "Nothing...";
        }
        else {
            return "Flip Pack";
        }
    }
    else{
        return "Flip Pack";
    }
}

bool GameCore::moveDtoS(){
    return standDeck->moveCard(standDeck, stacker);
}

bool GameCore::moveStoW(Working* w){
    return working1->moveCard(stacker, w);
}

bool GameCore::moveStoT(Target* t){
    return targetC->moveCard(stacker, t);
}

bool GameCore::moveMoreWtoW(Working* src, KCard* card, Working* dst){
    return working1->moveCards(src, card, dst);
}

bool GameCore::moveOneWtoW(Working* src, Working* dst){
    return working1->moveCard(src, dst);
}

bool GameCore::moveWtoT(Working* src, Target* t){
    return targetC->moveCard(src, t);
}

bool GameCore::moveTtoW(Target* src, Working* w) {
    return working1->moveCard(src, w);
}

void GameCore::undo() {
    globalUndo->makeUndo();
}

CardDeck* GameCore::getPack(){
    return standDeck;
}

CardDeck* GameCore::getStacker(){
    return stacker;
}

Target* GameCore::getTargetC(){
    return targetC;
}

Target* GameCore::getTargetS(){
    return targetS;
}

Target* GameCore::getTargetD(){
    return targetD;
}

Target* GameCore::getTargetH(){
    return targetH;
}

Working* GameCore::getWorking1(){
    return working1;
}

Working* GameCore::getWorking2(){
    return working2;
}

Working* GameCore::getWorking3(){
    return working3;
}

Working* GameCore::getWorking4(){
    return working4;
}

Working* GameCore::getWorking5(){
    return working5;
}

Working* GameCore::getWorking6(){
    return working6;
}

Working* GameCore::getWorking7(){
    return working7;
}
