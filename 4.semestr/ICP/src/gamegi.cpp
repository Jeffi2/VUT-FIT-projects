/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing one game in GUI
 * It contains all stacks, button and layout of the game
 *
 */

#include "gamegi.h"

#include <QMessageBox>
#include <QInputDialog>

GameGI::GameGI(QWidget *parent, MainWidget * form)
{
    game = new GameCore();
    win = parent;
    bigform = form;

    MainVBL =   new QVBoxLayout(win);
    Slots =     new QGridLayout();
    LbStatus =  new QLabel("Status: OK");
    Wrk_slot =  new QGridLayout();



    VbL =       new QVBoxLayout();
    BtnNew =    new QPushButton("New Game",win);
    BtnEnd =    new QPushButton("End Game",win);
    BtnHelp =   new QPushButton("Help",win);
    BtnUndo =   new QPushButton("Undo", win);

    Hbl =       new QHBoxLayout();
    BtnSave =   new QPushButton("Save",win);
    BtnLoad =   new QPushButton("Load",win);

    QObject::connect(BtnNew, SIGNAL(clicked()), this, SLOT(NewClick()));
    QObject::connect(BtnEnd, SIGNAL(clicked()), this, SLOT(EndClick()));
    QObject::connect(BtnHelp, SIGNAL(clicked()), this, SLOT(HelpClick()));
    QObject::connect(BtnUndo, SIGNAL(clicked()), this, SLOT(UndoClick()));
    QObject::connect(BtnSave, SIGNAL(clicked()), this, SLOT(SaveClick()));
    QObject::connect(BtnLoad, SIGNAL(clicked()), this, SLOT(LoadClick()));

    Wrk_slot->setContentsMargins(0,0,0,0);
    LbStatus->setContentsMargins(0,0,0,0);

    Pack =      new CardGI("P",win, game->getPack()->get());
    Stacker =   new CardGI("S",win, game->getStacker()->get());

    TargetH =   new CardGI("TH",win,game->getTargetH()->get());
    TargetD =   new CardGI("TD",win,game->getTargetD()->get());
    TargetC =   new CardGI("TC",win,game->getTargetC()->get());
    TargetS =   new CardGI("TS",win,game->getTargetS()->get());

    W1 = new WrkDckGI(win,this,game->getWorking1(),"W1",Wrk_slot,0,0);
    W2 = new WrkDckGI(win,this,game->getWorking2(),"W2",Wrk_slot,0,1);
    W3 = new WrkDckGI(win,this,game->getWorking3(),"W3",Wrk_slot,0,2);
    W4 = new WrkDckGI(win,this,game->getWorking4(),"W4",Wrk_slot,0,3);
    W5 = new WrkDckGI(win,this,game->getWorking5(),"W5",Wrk_slot,0,4);
    W6 = new WrkDckGI(win,this,game->getWorking6(),"W6",Wrk_slot,0,5);
    W7 = new WrkDckGI(win,this,game->getWorking7(),"W7",Wrk_slot,0,6);

    //QObject::connect(BtnEnd, SIGNAL(clicked()), this, SLOT(PackClick()));
    QObject::connect(Pack, SIGNAL(clicked(CardGI*)), this, SLOT(PackClick(CardGI*)));
    QObject::connect(Stacker, SIGNAL(clicked(CardGI*)), this , SLOT(DeckClick(CardGI*)));   
    QObject::connect(TargetC, SIGNAL(clicked(CardGI*)), this , SLOT(DeckClick(CardGI*)));
    QObject::connect(TargetH, SIGNAL(clicked(CardGI*)), this , SLOT(DeckClick(CardGI*)));
    QObject::connect(TargetD, SIGNAL(clicked(CardGI*)), this , SLOT(DeckClick(CardGI*)));
    QObject::connect(TargetS, SIGNAL(clicked(CardGI*)), this , SLOT(DeckClick(CardGI*)));


    MainVBL->addSpacing(10);
    MainVBL->addLayout(Slots);


    MainVBL->addSpacing(50);
    MainVBL->addLayout(Wrk_slot);

    MainVBL->addSpacing(250);
    MainVBL->addWidget(LbStatus);



    Slots->addWidget(Pack,0,0, Qt::AlignCenter);
    Slots->addWidget(Stacker,0,1,Qt::AlignCenter);

    Slots->addLayout(VbL,0,2,Qt::AlignCenter);
    VbL->addWidget(BtnNew,0,Qt::AlignCenter);
    VbL->addWidget(BtnEnd,0,Qt::AlignCenter);
    VbL->addWidget(BtnHelp,0,Qt::AlignCenter);
    VbL->addWidget(BtnUndo,0,Qt::AlignCenter);

    VbL->addLayout(Hbl);
    Hbl->addWidget(BtnSave,0,Qt::AlignCenter);
    Hbl->addWidget(BtnLoad,0,Qt::AlignCenter);

    Slots->addWidget(TargetH,0,3,Qt::AlignCenter);
    Slots->addWidget(TargetD,0,4,Qt::AlignCenter);
    Slots->addWidget(TargetC,0,5,Qt::AlignCenter);
    Slots->addWidget(TargetS,0,6,Qt::AlignCenter);
}

GameGI::~GameGI()
{

    delete game;

    delete MainVBL;

    delete LbStatus;

    delete BtnEnd;
    delete BtnNew;
    delete BtnUndo;
    delete BtnHelp;
    delete BtnSave;
    delete BtnLoad;
    //delete Hbl; //Pokud se snažím to vymazat, tak to spadne .. nevím proč


    delete Pack;
    delete Stacker;

    delete TargetH;
    delete TargetD;
    delete TargetC;
    delete TargetS;

    delete W1;
    delete W2;
    delete W3;
    delete W4;
    delete W5;
    delete W6;
    delete W7;

    //std::cout << "Tady\n" <<std::flush;
    //TADY TO SAMÉ
    //if(VbL != nullptr)
    //    delete VbL;

    //delete VbL;       //TODO
    //delete Wrk_slot; //TODO
    //delete Slots; //TODO
    //std::cout << "Tady\n" <<std::flush;

}

void GameGI::Rendering()
{
    Pack->rewrite(game->getPack()->get());
    Stacker->rewrite(game->getStacker()->get());

    TargetC->rewrite(game->getTargetC()->get());
    TargetD->rewrite(game->getTargetD()->get());
    TargetH->rewrite(game->getTargetH()->get());
    TargetS->rewrite(game->getTargetS()->get());

    W1->rewrite(game->getWorking1());
    W2->rewrite(game->getWorking2());
    W3->rewrite(game->getWorking3());
    W4->rewrite(game->getWorking4());
    W5->rewrite(game->getWorking5());
    W6->rewrite(game->getWorking6());
    W7->rewrite(game->getWorking7());

    //game->print_cli();

    if (game->winGame())
    {
        QString help_str = "Congratulation YOU WIN :)";
        QMessageBox::information(0,"WIN",help_str,QMessageBox::Ok,QMessageBox::NoButton);

        EndClick();
    }
}

void GameGI::PackClick(CardGI * ClkdCard)
{
    if(FirstCard != nullptr)
    {
        return;
    }

    game->moveDtoS();
    Pack->rewrite(game->getPack()->get());
    Stacker->rewrite(game->getStacker()->get());
}

Working* GameGI::whichWorking(QString str)
{
    //str.erase(str.begin());
    QChar wrk_i = str.at(1);

    switch (wrk_i.toLatin1())
    {
        case '1':
            return game->getWorking1();

        case '2':
            return game->getWorking2();

        case '3':
            return game->getWorking3();

        case '4':
            return game->getWorking4();

        case '5':
            return game->getWorking5();

        case '6':
            return game->getWorking6();

        case '7':
            return game->getWorking7();

        default:
            return nullptr;
    }
}

Target* GameGI::whichTarget(QString str)
{
   // str.erase(str.begin());
    QChar trg_c = str.at(1);
    switch (trg_c.toLatin1())
    {
        case 'C':
            return game->getTargetC();

        case 'H':
            return game->getTargetH();

        case 'D':
            return game->getTargetD();

        case 'S':
            return game->getTargetS();

        default:
            return nullptr;
    }
}


void GameGI::DeckClick(CardGI * ClkdCard)
{
    bool flag = false;

    //std::cout << ClkdCard->CardStack.toUtf8().constData() << std::endl;
    QString Status = "Status: ";

    if(FirstCard == nullptr)
    {

        if(ClkdCard->card_loc == nullptr || (!ClkdCard->card_loc->isTurnedFaceUp()))
            return;

        FirstCard = ClkdCard;

        //std::cout<<"Selected Card: "<<ClkdCard->card_loc->value()<<" "<<ClkdCard->card_loc->color()<<std::endl;

        Status += "select OK\t\tSelected card: " + ClkdCard->getCardName();
        LbStatus->setText(Status);
    }
    else
    {
        if(FirstCard != ClkdCard)
        {
            QStringList str_list;
            QString split_str;
            Working * wrk;
            Working * wrk2;
            int index;
            int index2;

            //std::cout<<"To Card: "<<ClkdCard->CardStack.toUtf8().constData()<<std::endl;

            QChar deck_char = FirstCard->CardStack.at(0);
            QChar deck_char2 = ClkdCard->CardStack.at(0);
            switch (deck_char.toLatin1())
            {
                case 'S':
                    switch (deck_char2.toLatin1())
                    {
                        case 'W':
                            str_list = ClkdCard->CardStack.split(",");
                            split_str = str_list.value(str_list.length()-1);
                            index = split_str.toInt();
                            wrk = this->whichWorking(ClkdCard->CardStack);

                            if(index > -1)
                                if(!ClkdCard->card_loc->isTurnedFaceUp())
                                    return;

                            if(index == (wrk->size()-1))
                                flag = game->moveStoW(this->whichWorking(ClkdCard->CardStack));

                            break;

                        case 'T':
                            flag = game->moveStoT(this->whichTarget(ClkdCard->CardStack));
                            break;
                        default:
                            break;
                    }
                    break;

                case 'W':
                    str_list = FirstCard->CardStack.split(",");
                    split_str = str_list.value(str_list.length()-1);
                    index = split_str.toInt();
                    wrk = this->whichWorking(FirstCard->CardStack);

                    switch (deck_char2.toLatin1())
                     {
                        case 'T':
                            if(index == (wrk->size()-1))
                            {
                                flag = game->moveWtoT(wrk,this->whichTarget(ClkdCard->CardStack));
                            }
                            break;

                        case 'W':
                            str_list = ClkdCard->CardStack.split(",");
                            split_str = str_list.value(str_list.length()-1);
                            index2 = split_str.toInt();
                            wrk2 = this->whichWorking(ClkdCard->CardStack);

                            if(index2 > -1)
                                if(!ClkdCard->card_loc->isTurnedFaceUp())
                                    return;

                            if(index2 == (wrk2->size()-1))
                            {
                                flag = game->moveMoreWtoW(wrk, FirstCard->card_loc,wrk2);
                            }
                            break;

                        default:
                            break;
                    }
                    break;

            case 'T':
                switch (deck_char2.toLatin1())
                {
                    case 'W':
                        str_list = ClkdCard->CardStack.split(",");
                        split_str = str_list.value(str_list.length()-1);
                        index = split_str.toInt();
                        wrk = this->whichWorking(ClkdCard->CardStack);

                        if(index > -1)
                            if(!ClkdCard->card_loc->isTurnedFaceUp())
                                return;

                        if(index == (wrk->size()-1))
                            flag = game->moveTtoW(this->whichTarget(FirstCard->CardStack), wrk);
                        break;
                }
                break;

                default:
                    break;
            }
        }
        else
        {
            this->DoublDecClick(ClkdCard);
            return;
        }

        FirstCard = nullptr;
        

        if (flag)
            Status += "move OK \t\t";
        else
            Status += "move INVALID\t";

        Status += "Selected card: No card!";
        LbStatus->setText(Status);

        this->Rendering();
    }
}

Target * GameGI::getTarget(KCard * card)
{
    int int_color = card->color();
    switch (int_color) {
        case 0:
            return whichTarget("TC");
        case 1:
            return whichTarget("TS");
        case 2:
            return whichTarget("TH");
        case 3:
            return whichTarget("TD");
        default:
            break;
    }

    return nullptr;
}


void GameGI::DoublDecClick(CardGI *ClkdCard)
{
    if(ClkdCard->card_loc == nullptr)
        return;

    if((!ClkdCard->card_loc->isTurnedFaceUp()))
        return;

    QChar stack = ClkdCard->CardStack.at(0);
    bool flag = false;

    Working * wrk;

    QStringList str_list;
    QString split_str;
    int index;

    switch (stack.toLatin1())
    {
        case 'S':
            flag = game->moveStoT(getTarget(ClkdCard->card_loc));
            break;

        case 'W':
            str_list = ClkdCard->CardStack.split(",");
            split_str = str_list.value(str_list.length()-1);
            index = split_str.toInt();
            wrk = this->whichWorking(ClkdCard->CardStack);

            if(index == (wrk->size()-1))
                flag = game->moveWtoT(wrk, this->getTarget(ClkdCard->card_loc));
            break;

        default:
            break;
    }

    FirstCard = nullptr;
    

    QString Status = "Status: ";

    if (flag)
        Status += "move OK \t\t";
    else
        Status += "NO move \t\t";

    Status += "Selected card: No card!";
    LbStatus->setText(Status);

    this->Rendering();


}

void GameGI::NewClick()
{
    bigform->NewGClick();
}

void GameGI::EndClick()
{
    bigform->EndGClick(this);
}

void GameGI::HelpClick()
{
    QString help_str = "You can move: " +  QString::fromStdString(game->getHelp());
    QMessageBox::information(0,"Help",help_str,QMessageBox::Ok,QMessageBox::NoButton);
}

void GameGI::UndoClick()
{
    game->undo();

    FirstCard = nullptr;
    QString stat = "Status: undo OK\t\tSelected card: No card!";
    LbStatus->setText(stat);

    this->Rendering();
}

void GameGI::SaveClick()
{
    QInputDialog input;
    bool ok;
    input.setOptions(QInputDialog::NoButtons);

    QString text =  input.getText(0 ,"Save game","Name of the save game:", QLineEdit::Normal,"game",&ok);

    if(ok)
    {
        QString stat = "Status: save ";

        if(!text.isEmpty())
        {
            text = text + ".save";

            if(game->saveGame(text.toUtf8().constData()))
                stat += "OK\t\t";
            else
                stat += "INVALID\t";
        }
        else
        {
            stat += "INVALID\t";
        }

        FirstCard = nullptr;
        stat += "Selected card: No card!";
        LbStatus->setText(stat);
    }
}

void GameGI::LoadClick()
{
    QInputDialog input;
    bool ok;
    input.setOptions(QInputDialog::NoButtons);

    QString text =  input.getText(0 ,"Load game","Name of the game, which you want to load:", QLineEdit::Normal,"../examples/4moves",&ok);

    if(ok)
    {
        QString stat = "Status: load ";

        if(!text.isEmpty())
        {
            text = text + ".save";

            if(game->loadGame(text.toUtf8().constData()))
            {
               stat += "OK\t\t";
               this->Rendering();
            }
            else
                stat += "INVALID\t";
        }
        else
        {
            stat += "INVALID\t";
        }

        FirstCard = nullptr;
        stat += "Selected card: No card!";
        LbStatus->setText(stat);
    }
}
