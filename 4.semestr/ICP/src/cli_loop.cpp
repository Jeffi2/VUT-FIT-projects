/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * CLI loop controller.
 *
 */

#include <stdlib.h>
#include <string>
#include "cli_loop.h"

int actual_game = 0;

Working* whichWorking(std::string str, GameCore* game)
{
    if(str.at(0) != 'W')
        return nullptr;

    str.erase(str.begin());
    char wrk_i = str.at(0);

    switch (wrk_i)
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

Target* whichTarget(std::string str, GameCore* game)
{
    if(str.at(0) != 'T')
        return nullptr;

    str.erase(str.begin());
    char trg_c = str.at(0);
    switch (trg_c)
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

int card_val(char c)
{
    switch(c)
    {
        case 'A':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'J':
            return 11;
        case 'Q':
            return 12;
        case 'K':
            return 13;

        default:
            return -1;
    }
}

CardColor::Color card_col(char c, bool* uncorect)
{
    *uncorect = false;
    switch (c)
    {
        case 'C':
            return CardColor::clubs;

        case 'S':
            return CardColor::spides;

        case 'H':
            return CardColor::hearts;

        case 'D':
            return CardColor::diamonds;

        default:
            *uncorect = true;
            return CardColor::diamonds;
    }
}

bool card_fStack(std::string str, GameCore * game)
{
    if(str.size() < 2)
        return false;


    if(str.at(0) == 'W')
    {
        Working* wrk = whichWorking(str,game);
        if (wrk == nullptr)
            return false;

        return game->moveStoW(wrk);
    }

    if(str.at(0) == 'T')
    {
        Target* trg = whichTarget(str,game);
        if (trg == nullptr)
            return false;

        return game->moveStoT(trg);
    }
    return false; //unknown operation
}

bool card_fWrk_up(std::string str, GameCore * game, Working * wrk)
{
    if(wrk == nullptr)
        return false;

    if(str.size() < 2)
        return false;

    if(str.at(0) == 'T')
    {
        Target* trg = whichTarget(str,game);

        if (trg == nullptr)
            return false;

        return game->moveWtoT(wrk, trg);
    }

    if(str.at(0) == 'W')
    {
        Working* wrk2 = whichWorking(str,game);
        if (wrk2 == nullptr)
            return false;

        return game->moveOneWtoW(wrk, wrk2);
    }

    return false; //unknown operation
}

bool card_fWrk_indx(std::string str, GameCore * game, Working * wrk, int index)
{
    if(wrk == nullptr)
        return false;

    if(str.size() < 2)
        return false;

    if(index == wrk->size()-1)
        return card_fWrk_up(str, game, wrk);

    if(str.at(0) == 'W')
    {
        Working* wrk2 = whichWorking(str,game);
        if (wrk2 == nullptr)
            return false;

        game->moveMoreWtoW(wrk, wrk->get(index), wrk2);
        return true;
    }

    return false;
}

bool card_fTarget(std::string str, GameCore * game)
{
    if(str.size() < 4)
        return false;


    if(str.at(0) == 'T')
    {
        Target* trg = whichTarget(str,game);
        if (trg == nullptr)
            return false;

        str = str.substr(3);

        if(str.at(0) == 'W')
        {
            Working* wrk = whichWorking(str,game);
            if (wrk == nullptr)
                return false;

            game->moveTtoW(trg, wrk);
            return true;
        }
    }

    return false; //unknown operation
}

int find_index (Working * wrk, KCard * my_card)
{
    if(wrk->isEmpty())
        return -1;

    KCard * card;

    for (int j = 0; j < wrk->size(); ++j)
    {
        card = wrk->get(j);
        if(card->same_card(my_card))
            return j;
    }

    return -1;
}

bool move_with_Card(std::string str, GameCore * game)
{
    int val = -1;
    CardColor::Color color = CardColor::diamonds;
    bool uncorrect_color = true;

    if(str.size() == 6)
    {
        if(str.at(0) != '1' || str.at(1) != '0')
            return false;

        val = 10;
        color = card_col(str.at(2), &uncorrect_color);
        str = str.substr(4);
    }
    else
    {
        if (str.size() == 5)
        {
            val = card_val(str.at(0));
            color = card_col(str.at(1), &uncorrect_color);
            str = str.substr(3);
        }
    }

    if(val == -1 || uncorrect_color)
        return false;

    KCard my_card(color,val);
    KCard* card;

    //card on stack
    if(!game->getStacker()->isEmpty())
    {
        card = game->getStacker()->get();
        if(card->same_card(&my_card))
        {
            return card_fStack(str, game);
        }
    }

    //check TargetD
    if(!game->getTargetD()->isEmpty())
    {
        card = game->getTargetD()->get();
        if(card->same_card(&my_card))
        {
            return card_fTarget("TD "+str, game);
        }
    }

    //check TargetH
    if(!game->getTargetH()->isEmpty())
    {
        card = game->getTargetH()->get();
        if(card->same_card(&my_card))
        {
            return card_fTarget("TH "+str, game);
        }
    }

    //check TargetC
    if(!game->getTargetC()->isEmpty())
    {
        card = game->getTargetC()->get();
        if(card->same_card(&my_card))
        {
            return card_fTarget("TC "+str, game);
        }
    }

    //check TargetS
    if(!game->getTargetS()->isEmpty())
    {
        card = game->getTargetS()->get();
        if(card->same_card(&my_card))
        {
            return card_fTarget("TS "+str, game);
        }
    }

    int index;
    //worikng 1
    index = find_index(game->getWorking1(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking1(),index);
    }

    //worikng 2
    index = find_index(game->getWorking2(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking2(),index);
    }

    //worikng 3
    index = find_index(game->getWorking3(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking3(),index);
    }

    //worikng 4
    index = find_index(game->getWorking4(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking4(),index);
    }

    //worikng 5
    index = find_index(game->getWorking5(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking5(),index);
    }

    //worikng 6
    index = find_index(game->getWorking6(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking6(),index);
    }

    //worikng 7
    index = find_index(game->getWorking7(),&my_card);
    if(index != -1)
    {
        return card_fWrk_indx(str,game,game->getWorking7(),index);
    }

    return false;

}

void activate_game(int n, GameCore ** games)
{
    if(games[n] == nullptr)
    {
        std::cout << "JSEM ZDE! \n";
        games[n] = new GameCore();
    }

    actual_game = n;
    std::cout << "ACTUAL GAME: " << actual_game << "\n";
}


int parse_cmd(std::string input_str, GameCore ** game)
{
    if(input_str.compare("H") == 0) //for print help
        return 0;

    if(input_str.compare("E") == 0) //for EXIT one game
        return -1;

    if(input_str.compare("EALL") == 0) //for EXIT all games
        return -2;

    if(input_str.compare("Q") == 0)
        return 1;

    if(input_str.compare("Help") == 0)
        return 2;

    //Save for SAVE
    if(input_str.compare("S") == 0)
    {
        std::cout << "Enter the name of the saved game: ";
        std::string file_name;
        std::getline (std::cin,file_name);

        file_name = file_name+".save";

        std::cout << "GAME SAVE: ";
        if (game[actual_game]->saveGame(file_name))
            return 4;
        else
            return 5;
    }

    //Load for LOAD
    if(input_str.compare("L") == 0)
    {
        std::cout << "Enter the name of the game, which you want to load: ";
        std::string file_name;
        std::getline (std::cin,file_name);

        file_name = file_name+".save";

        std::cout << "GAME LOAD: ";

        if (game[actual_game]->loadGame(file_name))
        {
            std::cout << "operation SUCCESSDULLY completed!\n"<<std::flush;
            return 1;
        }
        else
            return 5;
    }

    if(input_str.compare("U") == 0)
    {
        game[actual_game]->undo();
        return 1;
    }

    if(input_str.compare("P") == 0)
    {
        game[actual_game]->moveDtoS();
        return 1;
    }

    if(input_str.size() > 1)
    {

        Working * wrk1;
        switch (input_str.at(0))
        {
            case 'P':
                if(input_str.size() != 4)
                    return 2;

                if(card_fStack(input_str.substr(2), game[actual_game]))
                    return 1;
                else
                    return 2;

            case 'T':
                if(input_str.size() != 5)
                    return 2;

                if(card_fTarget(input_str, game[actual_game]))
                    return 1;
                else
                    return 2;

            case 'W':
                if(input_str.size() != 5)
                    return 2;

                wrk1 = whichWorking(input_str,game[actual_game]);
                if(wrk1 == nullptr)
                    return 2;

                if(card_fWrk_up(input_str.substr(3), game[actual_game], wrk1))
                    return 1;
                else
                    return 2;

            case 'G':
                if (input_str.size() != 2)
                    return 2;

                switch (input_str.at(1))
                {
                    case '0':
                        activate_game(0, game);
                        break;

                    case '1':
                        activate_game(1, game);
                        break;

                    case '2':
                        activate_game(2, game);
                        break;

                    case '3':
                        activate_game(3, game);
                        break;

                    default:
                        return 3;
                }
                return 1;

            default:
                if(!((input_str.size() == 5) || (input_str.size() == 6)))
                    return 2;


                //std:: cout
                bool moving = move_with_Card(input_str, game[actual_game]);

                if(!moving)
                    return 2;

                return 1;
        }
    }

    return 2;
}

void print_help()
{
    system("tput clear");
    system("cat help_cli.txt");
}


void cli_loop(GameCore ** game)
{
    actual_game = 0;

    game[actual_game]->print_cli();
    int i_cmd=0;


    for (std::string cmd; (std::getline(std::cin, cmd));)
    {
        i_cmd = parse_cmd(cmd, game);
        bool test = false;
        int i = 0;
        for (i = 0; i < 4; ++i) {
            if (game[i] != nullptr && i != actual_game) {
                test = true;
                break;
            }
        }

       switch (i_cmd)
       {
           case -2:
               std::cout << "Exiting all games..." << std::endl;
               return; //EALL
               break;
           case -1:
               if (test){
                   std::cout << "Game: " << actual_game << " EXITED, switch to game: " << i << std::endl;
                   delete game[actual_game];
                   game[actual_game] = nullptr;
                   actual_game = i;
                   game[actual_game]->print_cli();
               }
               else {
                   std::cout << "\nEXIT : GOOD BAY :) \n";
                   return;
               }
               break;
           case 0:
               std::cout << "\nYOU CAN PLAY: "+game[actual_game]->getHelp()+"\ncmd: ";
               break;

           case 1:
               if (game[actual_game]->winGame()){
                   if (test) {
                       std::cout << "\nCongratulation YOU WIN Game: " << actual_game << " switch to game: " << i << std::endl;
                       delete game[actual_game];
                       game[actual_game] = nullptr;
                       actual_game = i;
                   }
                   else {
                       std::cout << "\nCongratulation YOU WIN :)" << std::endl;
                       return;
                   }
               }
               game[actual_game]->print_cli();
               break;
           case 2:
               print_help();
               break;

           case 4:
               std::cout << "operation SUCCESSFULLY completed!\ncmd: ";
               break;

           case 5:
               std::cout << "operation was UNSUCCESSFUL!\ncmd: ";
               break;

           default:
            std::cout << "Unknown command! Try it again!" << std::endl << "cmd: ";
               break;
       }

    }
}