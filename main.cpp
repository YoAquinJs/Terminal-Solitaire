#include <iostream>
#include <string>
#include <vector>

#include "magic_enum/magic_enum.hpp"

#include "classes/game.hpp"
#include "classes/deck.hpp"
#include "classes/column.hpp"
#include "classes/drawPile.hpp"
#include "classes/foundation.hpp"
#include "gameLogic/gameLogic.hpp"

//Run
//g++ main.cpp gameLogic/*.cpp classes/*.cpp -o main && ./main
int main(){
	const int tablueColumns = 7;
	const int drawDeck = 52-(tablueColumns*(tablueColumns+1)/2);

	//Deck
	Deck deck;
	deck.Shuffle();

	//Game Object
	Game game;

	//Sections
	std::cout << drawDeck << std::endl;
	game.drawSection->AddPile(new DrawPile(deck.GetRange(drawDeck)));
	game.drawSection->AddPile(new CardPile(std::vector<Card*>()));
	std::cout << "work1" << std::endl;

	for (int i = 0; i < magic_enum::enum_count<CardSuit>(); i++){
		game.foundationSection->AddPile(new Foundation());
	}

	for (int i = 1; i <= tablueColumns; i++){
		game.tableuSection->AddPile(new Column(deck.GetRange(i)));
	}

	//Section links
	game.drawSection->LinkSurroundingSection(Direction(1,0), game.foundationSection);
	game.tableuSection->LinkSurroundingSection(Direction(0,1), game.tableuSection);
	game.tableuSection->LinkSurroundingSection(Direction(0,1), game.tableuSection);

	//Game Loop
	Render(&game);
	Update(&game);
	bool inGame = true;
	while (inGame){
	}

	return 0;
}