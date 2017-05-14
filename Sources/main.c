#include <stdio.h>
#include <game.h>


int main(){

	int error = init_game(600,400);
	if (error)
	{
		printf("Error!");
		return 1;
	}

	start_game();
	closing_game();

	return 0;
}