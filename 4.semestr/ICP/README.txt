Game Solitaire Klondike - as a project to ICP 2016/17

Autors: 
	Vladimír Jeřábek (xjerab21), Petr Jůda (xjudap00)

Game:
	Game is devided into two games with the same core. Is possible to run only the CLI version that supports same functionality as GUI game.

How to build:
	$ make

How to run:
	$ make run        	// This command run hra2017 and hra2017-cli in same time
	$ make rungui		// This command for only GUI version: hra2017
	$ make runcli		// This command for only CLI version: hra2017-cli


Folder structure:
./src/*      source files
./examples/* saved games
./doc/       documentation (generete by command: make doxygen)
./README.txt description
./Makefile   run Makefile for CLI and GUI or generate documentation

Game description: 
	The Solitaire is a classic card game know all over the world. Mostly because it's part of built in games in Microsoft Windows.
	Game goal is to get all cards to four 'target' stacks (each for one colour). But you can use only moves thats are declared 
	in game rules. Our implementation of the game allows to play up to four games at the same time. You can also use an undo 
	command that can move you back to start of the game. If you get stuck, the 'help' command will be helpful. You can also save
	the game to a file and load it back whenever you want.

Game rules:
	Game contains standard pack of 52 cards. Some of them are hidden at the start. 24 cards are hidden in "Pack" in the beginning
	and you can see them after move "Flip Pack". After that one of "Pack" card will show on "Stacker" and you can move one card to target
	or working stacks. Your goal is to get all cards to target stacks. There is four of target stacks and each of theme accepts 
	only cards of same colour. First card on the target stack must bean ace, second must be two, three... up to king. To get it possible
	there are seven of the working stacks that you can use for moving cards. If the stack is empty you can put a King of some colour to it. 
	On the king you can put a Queen but it must have different colour (recognise red and black cards). Between working stacks you can move
	more cards. But from working and target (or otherwise) you can move only one card.