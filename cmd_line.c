#include <stdio.h>
#include <stdbool.h>
#define ALIVE 1
#define DEAD 0
#define RESOLUTION_X 40
#define RESOLUTION_Y 40
bool game_board[RESOLUTION_X][RESOLUTION_Y];

void pulsar(int centre_x, int centre_y){
//	if (!check_bounds(centre_x+6, centre_y+6) || 
//		!check_bounds(centre_x-6, centre_y-6)) return;
	for (int d = 2; d <=4; d++)
	{
		game_board[centre_x+d][centre_y+6] = ALIVE;
		game_board[centre_x+d][centre_y+1] = ALIVE;
		game_board[centre_x+d][centre_y-6] = ALIVE;
		game_board[centre_x+d][centre_y-1] = ALIVE;
		game_board[centre_x-d][centre_y+6] = ALIVE;
		game_board[centre_x-d][centre_y+1] = ALIVE;
		game_board[centre_x-d][centre_y-6] = ALIVE;
		game_board[centre_x-d][centre_y-1] = ALIVE;
		
		game_board[centre_x+6][centre_y+d] = ALIVE;
		game_board[centre_x+1][centre_y+d] = ALIVE;
		game_board[centre_x-6][centre_y+d] = ALIVE;
		game_board[centre_x-1][centre_y+d] = ALIVE;
		game_board[centre_x+6][centre_y-d] = ALIVE;
		game_board[centre_x+1][centre_y-d] = ALIVE;
		game_board[centre_x-6][centre_y-d] = ALIVE;
		game_board[centre_x-1][centre_y-d] = ALIVE;
	}
}

void update_board_state(){
	bool new_board[RESOLUTION_X][RESOLUTION_Y];
    int total = 0,n = RESOLUTION_X,m = RESOLUTION_Y;
    for(int i=0;i < RESOLUTION_X;i++){
      for(int j=0;j < RESOLUTION_Y;j++){
        total = (game_board[(i-1)%n][(j-1)%m]+game_board[(i-1)%n][j]
        +game_board[(i-1)%n][(j+1)%m]+game_board[i][(j-1)%m]
        +game_board[i][(j+1)%m]+game_board[(i+1)%n][(j-1)%m]
        +game_board[(i+1)%n][j]+game_board[(i+1)%n][(j+1)%m])/ALIVE;

        //printf("%d %d:%d\n",i,j,total);

        if(game_board[i][j]==ALIVE){
            // cell dies if it has insufficient neighbours
            if(total < 2 || total > 3){
                new_board[i][j] = DEAD;
            }
			else{
				new_board[i][j] = ALIVE;
			}
        }else{
            // cell lives if it has 3 neighbours
            if(total==3){
                new_board[i][j] = ALIVE;
            }
			else{
				new_board[i][j] = DEAD;
			}
        }
      }
    }
    for(int i=0;i < RESOLUTION_X;i++){
      for(int j=0;j < RESOLUTION_Y;j++){
		 game_board[i][j] = new_board[i][j];
	  }
	}
}

int main(){
    pulsar(RESOLUTION_X/2,RESOLUTION_Y/2);
    game_board[12][12] = ALIVE;
    game_board[12][13] = ALIVE;
    game_board[12][14] = ALIVE;
    //write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
    while(true){
        for (int i = 0; i < RESOLUTION_X; i++)
        {
            for (int j = 0; j < RESOLUTION_Y; j++){
                printf((game_board[i][j]==ALIVE)?"#":".");
            }
            printf("\n");
        }
        getchar();
        update_board_state();
    }
    return 0;
}