#include <string>
#include <limits>
#include <cstdlib>
#include <iostream>

#include "../classes/foundation.hpp"
#include "../classes/drawPile.hpp"
#include "../classes/column.hpp"
#include "gameLogic.hpp"
#include "utils.hpp"

void WaitForEnter();
void FillRenderMatrix(Game* game);
bool WonGame(Game* game);
void PerformAction(Game* game, char input, int drawDeck);

void Update(Game* game, int drawDeckSize){
	switch (game->state){
		case GameState::START:
    		WaitForEnter();

			game->state = GameState::ONGAME;
			FillRenderMatrix(game);
			break;
		case GameState::ONGAME:
		{
			std::string rawInput;
			std::cin >> rawInput;

			if (rawInput.size() != 1){
				ColorPrint("Invalid Action, more than a single character", RED);
				return;
			}

			PerformAction(game, std::tolower(rawInput[0]), drawDeckSize);

			if (WonGame(game))
				game->state = GameState::WON;

			FillRenderMatrix(game);
		}break;
		case GameState::WON:
			WaitForEnter();

			exit(EXIT_SUCCESS);
			break;
	}
}


void WaitForEnter(){
	std::cin.clear();	
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.get();
}

void FillRenderMatrix(Game* game){
	//Empty space between draw section and foundation section
	game->renderMatrix[2][0] = nullptr;

	//Draw Section
	for (int i = 0; i < game->drawSection.Count(); i++){
		game->renderMatrix[i][0] = game->drawSection.GetAt(i)->GetAt(0);
	}

	//Foundation Section
	for (int i = 0; i < game->foundationSection.Count(); i++){
		game->renderMatrix[i+3][0] = game->foundationSection.GetAt(i)->GetAt(0);
	}

	//Tableu Section
	for (int i = 1; i < 14; i++){
		for (int j = 0; j < game->tableuSection.Count(); j++){
			Column* column = static_cast<Column*>(game->tableuSection.GetAt(j));
			game->renderMatrix[j][i] = game->tableuSection.GetAt(j)->GetAt(column->Count()-i);
		}
	}
}

void PerformAction(Game* game, char input, int drawDeckSize){
	auto getSectionFunc = [&game](CardPile* pile) {
    	return game->GetPileSection(pile);
	};
	switch (input){
		case 'w':
			if (game->cursor1->locked)
				game->cursor2->Move(Direction(0,1), getSectionFunc);
			else
				game->cursor1->Move(Direction(0,1), getSectionFunc);
			break;
		case 's':
			if (game->cursor1->locked)
				game->cursor2->Move(Direction(0,-1), getSectionFunc);
			else
				game->cursor1->Move(Direction(0,-1), getSectionFunc);
			break;
		case 'a':
			if (game->cursor1->locked)
				game->cursor2->Move(Direction(-1,0), getSectionFunc);
			else
				game->cursor1->Move(Direction(-1,0), getSectionFunc);
			break;
		case 'd':
			if (game->cursor1->locked)
				game->cursor2->Move(Direction(1,0), getSectionFunc);
			else
				game->cursor1->Move(Direction(1,0), getSectionFunc);
			break;

		case 'l':
			if (game->cursor1->locked){
				ColorPrint("Invalid Action, cannot lock both cursors", RED);
				WaitForEnter();
			}else
				game->cursor1->locked=true;
			break;
		case 'k':
			if (!game->cursor1->locked){
				ColorPrint("Invalid Action, cannot unlock cursor, when it's not locked", RED);
	    		WaitForEnter();
			}else
				game->cursor1->locked=false;
			break;

		case 'j':
			//Draw pile Interactions
			if (!game->cursor1->locked){
				if (!(game->cursor1->GetPile() == game->drawSection.GetAt(0)))
					break;

				DrawPile* drawPile = dynamic_cast<DrawPile*>(game->drawSection.GetAt(0));
				if (!drawPile->MoveCard(game->drawSection.GetAt(1))){
					for (int i = 0; i < drawDeckSize; i++){
						std::cout << game->drawSection.GetAt(1)->MoveCard(drawPile) <<std::endl;
						std::cout << game->drawSection.GetAt(1)->Count() <<"\n";
					}
					drawPile->InitPile();
				}
				break;
			}
			//Draw Pile-Column Iteraction
			if (game->cursor1->GetPile() == game->drawSection.GetAt(1) && game->cursor2->GetSection() == &game->tableuSection){
				game->cursor1->GetPile()->MoveCard(dynamic_cast<Column*>(game->cursor2->GetPile()));
				break;
			}
			//Draw Pile-Foundation Iteraction
			if (game->cursor1->GetPile() == game->drawSection.GetAt(1) && game->cursor2->GetSection() == &game->foundationSection){
				game->cursor1->GetPile()->MoveCard(dynamic_cast<Foundation*>(game->cursor2->GetPile()));
				break;
			}
			//Column-Column Interaction
			if (game->cursor1->GetSection() == &game->tableuSection && game->cursor1->GetSection() == &game->tableuSection){
				Column* destination = dynamic_cast<Column*>(game->cursor2->GetPile());
				dynamic_cast<Column*>(game->cursor1->GetPile())->MoveSubColumn(game->cursor1->GetIndex(), destination);
				break;
			}
			//Foundation-Column Interaction
			if (game->cursor1->GetSection() == &game->tableuSection && game->cursor2->GetSection() == &game->foundationSection){
				game->cursor1->GetPile()->MoveCard(dynamic_cast<Foundation*>(game->cursor2->GetPile()));
				break;
			}
			if (game->cursor1->GetSection() == &game->foundationSection && game->cursor2->GetSection() == &game->tableuSection){
				game->cursor1->GetPile()->MoveCard(dynamic_cast<Column*>(game->cursor2->GetPile()));
				break;
			}
			break;
				
		default:
			ColorPrint("Invalid Input: " + input, RED);
    		WaitForEnter();
			break;
		}
}

bool WonGame(Game* game){
	for (int i = 0; i < game->foundationSection.Count(); i++){
		if (game->foundationSection.GetAt(i)->Count() == 0 || game->foundationSection.GetAt(i)->GetAt(0)->GetRank() != CardRank::KING)
			return false;
	}

	return true;
}
