/* FPGA address macros */
#define BOARD                 "DE1-SoC"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define FPGA_BRIDGE           0xFFD0501C

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

/* processor mode constants */
#define		USER_MODE				0b10000
#define		FIQ_MODE				0b10001
#define		IRQ_MODE				0b10010
#define		SVC_MODE				0b10011
#define		ABORT_MODE				0b10111
#define		UNDEF_MODE				0b11011
#define		SYS_MODE				0b11111

#define 	KEYS_IRQ				73
#define   PS2_IRQ         79

#define		INT_ENABLE				0b01000000
#define		INT_DISABLE				0b11000000
#define		ENABLE					0x1

/* default VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BLACK 0x0

/* Screen and character buffer resolution */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240
#define CHAR_MAX_X 80
#define CHAR_MAX_Y 60

/* Mouse Input Settings */
#define ACCELERATION 1

/* Tile Active */
#define DEAD  0
#define ALIVE 1

/* define absolute value function */
#define ABS(x) (((x) > 0) ? (x) : -(x))

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <float.h>
#include <string.h>

volatile int pixel_buffer_start; // global variable

/* LinkedList definition start */
typedef struct node{
  int x;
  int y;
  bool isTile;
  struct node* next;
}Node;

typedef struct linkedlist{
  Node* head;
}LinkedList;

Node* createNode(int x,int y,bool isTile){
  Node* temp = (Node*)malloc(sizeof(Node));
  temp->x = x;
  temp->y = y;
  temp->next = NULL;
  temp->isTile = isTile;
  if(temp==NULL){
    printf("Error: Could not allocate memory for node");
  }
  return temp;
}

LinkedList* init(){
  LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
  list->head = NULL;
  if(list==NULL){
    printf("Error: Could not allocate memory for node");
  }
  return list;
}

bool isEmpty(LinkedList* list){
  return (list->head==NULL);
}

void insertFront(LinkedList* list,int x,int y,bool isTile){
  Node* temp = createNode(x,y,isTile);
  temp->next = list->head;
  list->head = temp;
}

void insertTail(LinkedList* list,int x,int y,bool isTile){
  if(isEmpty(list)){
    list->head=createNode(x,y,isTile);
    return;
  }
  Node* temp = list->head;
  while(temp->next!=NULL){
    temp=temp->next;
  }
  temp->next = createNode(x,y,isTile);
}

void deleteFront(LinkedList* list){
  Node* temp = list->head;
  list->head = list->head->next;
  free(temp);
}

bool deleteTail(LinkedList* list){
  if(isEmpty(list)){
    return false;
  }else if(list->head->next==NULL){
    deleteFront(list);
    return true;
  }
  Node* temp = list->head;
  while(temp->next->next!=NULL){
    temp=temp->next;
  }
  free(temp->next);
  temp->next = NULL;
  return true;
}

void deleteList(LinkedList* list){
  while(list->head!=NULL){
    deleteFront(list);
  }
}

void printList(LinkedList* list){
  if(isEmpty(list)){
    printf("Error: List is empty.\n");
    return;
  }
  Node* temp = list->head;
  do{
    printf("%d %d\n",temp->x,temp->y);
    temp=temp->next;
  }while(temp!=NULL);
  printf("\n");
}
/* LinkedList definition end */



/* GLOBAL VARIABLES */

volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

// global drawn pixels linked lists
LinkedList* pixel_list;
LinkedList* front_list;
LinkedList* back_list;

// game board
bool game_board[RESOLUTION_X][RESOLUTION_Y];
bool prev_board[RESOLUTION_X][RESOLUTION_Y];
int color_board[RESOLUTION_X][RESOLUTION_Y];

// global colour values
short int bg_color;
short int tile_color;

// game states
bool isPaused;

// linked tile size
int l_tile_size;

// mouse position and size variables
int mouse_x,mouse_y;
int mouse_width,mouse_height;

short int cursor_color;

// screen
volatile int screen;
enum Screens{MAIN_MENU, RANDOM, USER_DRAWING, PRESETS};


/* GLOBAL VARIABLES END */


/* GAME FUNCTIONS */

// program screens
void main_menu();
void random_screen();
void user_drawing();
void presets();
//void logic_gates();

// set the starting board state
void initialize_board();

// draw the game board (a rectangle in the screen)

void draw_board(int x0, int x1, int y0, int y1, int size);

// initialize the board randomly with prop % chance that a cell is alive at the start
void random_initialization(int x0, int x1, int y0, int y1, float prop);

// update and draw the board (a rectangle in the screen)
void update_board_state(int x0, int x1, int y0, int y1, int size);

// shapes and creatures:
void ship(int centre_x, int centre_y);
void block(int left_x, int top_y);
void tub(int centre_x, int centre_y);
void toad(int left_x, int top_y);
void blinker(int centre_x, int centre_y);
void glider(int left_x, int top_y);
void pulsar(int centre_x, int centre_y);
void draw_ECE243(int left_x, int top_y);

void glider_eater(int left_x, int top_y, bool hor_not_vert);
void glider_gun(int left_x, int top_y, bool hor_not_vert);
void and_gate(int left_x, int top_y);
void or_gate(int left_x, int top_y);
void not_gate(int left_x, int top_y);
/* GAME FUNCTIONS END */


/* HELPER FUNCTIONS */
// set up IRQ stack pointer
void set_up_IRQ();

// configure GIC
void config_GIC();

// configure keys
void config_KEYs();

//void config_SWs();

// interrupt service routine for KEYs
void KEY_ISR();

// configure mouse input (PS2 port)
void config_ps2();

// interrupt service routine for mouse input (PS2 port)
void MOUSE_ISR();

// interrupt mouse coordinates
void initialize_mouse_coords();

// parse left mouse button clicked procedures
void parse_lmb_click();

// parse right mouse button clicked procedures
void parse_rmb_click();

// check if mouse is in bounds (x1<x2,y1<y2)
bool mouse_bounds(int x1,int y1,int x2,int y2);

// update mouse coords and keep mouse position within screen bounds
void update_mouse_coords(int disp_x,int disp_y);

//void SW_ISR();

// enable interrupts after setting up devices
void enable_interrupts();

// plot a pixel with given colour at coordinates
void draw_pixel(int x, int y, short int line_color);

void draw_tile(int x, int y, int size, short int line_color);

void draw_tile_clear(int x,int y,int size,short int line_color);

// draw a string of text on character buffer
void draw_text(int x, int y, char * text_ptr);

// draw the cursor on the screen
void draw_cursor();

// wait for vertical synchronization
void wait_for_vsync();

// clear the screen on startup (slow clear)
void initial_clear();

// clears the character buffer
void initial_clear_chars();

// clear the screen between frames (fast clear);
void clear_screen(LinkedList* list,int size);

// clear everything
void clear(int tile_size);

// check if coordinates is on game board
bool check_bounds(int x,int y,int size);


/* HELPER FUNCTIONS END */


int main(void)
{
    /* Initialize variables */
    bg_color = BLACK;
    tile_color = WHITE;
    cursor_color = GREEN;
    isPaused = true;

    pixel_list = init();
    back_list = init();
    front_list = init();

	//srand(time(NULL));

	set_up_IRQ();
	config_GIC();
	config_KEYs();
  config_ps2();
  initialize_mouse_coords();
	enable_interrupts();

	initial_clear_chars();

	/* Set up and clear both front and back buffers */
    // set front pixel buffer to start of FPGA On-chip memory
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
                                        // back buffer
    // now, swap the front/back buffers, to set the front buffer location
    wait_for_vsync();
    // initialize a pointer to the pixel buffer, used by drawing functions
    pixel_buffer_start = *pixel_ctrl_ptr;
    initial_clear();

     // pixel_buffer_start points to the pixel buffer
    // set back pixel buffer to start of SDRAM memory
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    initial_clear();

	screen = MAIN_MENU;

	main_menu();
}

void main_menu(){
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear_chars();

	const int tile_size = 5; // 5x5
	const int board_x = RESOLUTION_X/tile_size;
	const int board_y = RESOLUTION_Y/tile_size;
	isPaused = true;

	// Title and menu options
	char* str = "Game of Life\0";
	draw_text((80-strlen(str))/2, 1,str);
	str = "Welcome to Conway's Game of Life.\0";
	draw_text((80-strlen(str))/2, 4, str);
	str = "Press pushbutton 0 to start simulation.\0";
	draw_text(4, 6, str);
	str = "Use switches or the mouse to select a menu option.\0";
	draw_text(4, 8, str);
	char* credits = "Stephen and Yvonne, 2021\0";
	draw_text(55,58,credits);

	str = "1. Random\0";
	draw_text(30, 12, str);
	str = "2. Draw your own\0";
	draw_text(30, 16, str);
	str = "3. Presets\0";
	draw_text(30, 20, str);

	draw_ECE243(11, 30);

	// simulation loop
	int total_iterations = 0;
    while (1)
    {
		if (screen != MAIN_MENU) break;
		if (total_iterations > 0 && isPaused) // display the initial config
			continue;

        // start clearing previously drawn pixels
    		clear(tile_size);
        // finish clearing previously drawn pixels

        draw_cursor();

        // draw, then update the game board
        update_board_state(0,board_x,0,board_y, tile_size);

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

		total_iterations++;
    }
	if (screen == RANDOM)
		random_screen();
	else if (screen == USER_DRAWING)
		user_drawing();
	else if (screen == PRESETS)
		presets();
	else
		;
}

void random_screen(){
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear_chars();

	const int tile_size = 6;
	const int board_x0 = 15/tile_size;
	const int board_y0 = 40/tile_size;
	const int board_x1 = (RESOLUTION_X-15)/tile_size;
	const int board_y1 = (RESOLUTION_Y-25)/tile_size;
	isPaused = true;

	// Title and instructions
	char* str = "Game of Life Random Generator\0";
	draw_text((80-strlen(str))/2, 1,str);
	str = "Enjoy a simulation of Life with randomly generated tiles.\0";
	draw_text(4, 4, str);
	str = "Press pushbutton 0 to start simulation.\0";
	draw_text(4, 6, str);
	str = "Back to Main Menu<<\0"; // clicky stuff?
	draw_text(2, 57, str);
	char* credits = "Stephen and Yvonne, 2021\0";
	draw_text(55,58,credits);

	random_initialization(board_x0, board_x1, board_y0, board_y1, 0.9);

	// simulation loop
	int total_iterations = 0;
    while (1)
    {
		if (screen != RANDOM) break;
		if (total_iterations > 0 && isPaused) // display the initial config
			continue;

        // start clearing previously drawn pixels
        clear(tile_size);
        // finish clearing previously drawn pixels

        // draw, then update the game board
        update_board_state(board_x0,board_x1,board_y0,board_y1, tile_size);

        draw_cursor();

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

		total_iterations++;
    }
	if (screen == USER_DRAWING)
		user_drawing();
	else if (screen == PRESETS)
		presets();
  else if (screen == MAIN_MENU)
    main_menu();
	else
		;


}

void user_drawing(){
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear_chars();

	const int tile_size = 5; // 5x5
	const int board_x = RESOLUTION_X/tile_size;
	const int board_y = RESOLUTION_Y/tile_size;
	isPaused = true;

  l_tile_size = tile_size;

	// Title and instructions
	char* str = "Game of Life - Draw your own\0";
	draw_text((80-strlen(str))/2, 1,str);
	str = "Drag the mouse on the screen to draw an initial configuration.\0";
	draw_text(4, 4, str);
	str = "Press pushbutton 0 to start simulation.\0";
	draw_text(4, 6, str);
	str = "Back to Main Menu<<\0"; // clicky stuff?
	draw_text(2, 57, str);
	char* credits = "Stephen and Yvonne, 2021\0";
	draw_text(55,58,credits);

	// simulation loop
	int total_iterations = 0;
    while (1)
    {
		if (screen != USER_DRAWING) break;

        // start clearing previously drawn pixels
		    clear(tile_size);
        // finish clearing previously drawn pixels

        // draw, then update the game board
        if(!isPaused){
            update_board_state(0,board_x,0,board_y,tile_size);
        }else{
            draw_board(0,board_x,0,board_y,tile_size);
        }

        draw_cursor();

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

		total_iterations++;
    }

  // reset linked tile size
  l_tile_size = 0;

	if (screen == RANDOM)
		random_screen();
	else if (screen == PRESETS)
		presets();
  else if (screen == MAIN_MENU)
    main_menu();
	else
		;

}

void presets(){
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	initial_clear_chars();

	const int tile_size = 2; // 2x2
	const int board_x = RESOLUTION_X/tile_size;
	const int board_y = RESOLUTION_Y/tile_size;
	isPaused = true;

	// Title and instructions
	char* str = "Game of Life Presets\0";
	draw_text((80-strlen(str))/2, 1,str);
	str = "Welcome to the gallery of Life creatures.\0";
	draw_text(4, 4, str);
	str = "Press pushbutton 0 to start simulation.\0";
	draw_text(4, 6, str);
	str = "Back to Main Menu<<\0"; // clicky stuff?
	draw_text(2, 57, str);
	char* credits = "Stephen and Yvonne, 2021\0";
	draw_text(55,58,credits);

    // 1. Still Life

	str = "1. Still Life\0";
	draw_text(4, 10, str);
	block(49, 25);
	tub(60, 25);
	ship(70, 25);

	// 2. Oscillators
	str = "2. Oscillators\0";
	draw_text(4, 20, str);

	blinker(50, 50);
	toad(60, 49);
	pulsar(80, 50);

	// 3. Space ships and Guns
	str = "3. Space ships and Guns\0";
	draw_text(4, 30, str);
	glider(50, 70);
	glider_gun(80, 70, true);

	/*

	// 4. Logic gates
	str = "4. Logic gates and other cool stuff\0";
	draw_text(4, 50, str);

	draw_ECE243(200, 200);

	/* // scratch this, this is kinda stupid
	draw_board(0,RESOLUTION_X,0,RESOLUTION_Y);
    wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	draw_board(0,RESOLUTION_X,0,RESOLUTION_Y);
    wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	draw_ECE243(100, 100);
	*/

	// simulation loop
	int total_iterations = 0;
    while (1)
    {
		if (screen != PRESETS) break;
		if (total_iterations > 0 && isPaused) // display the initial config
			continue;

        // start clearing previously drawn pixels
        clear(tile_size);
        // finish clearing previously drawn pixels

        // draw, then update the game board
        //update_board_state(0,RESOLUTION_X,0,RESOLUTION_Y);

        update_board_state(0,board_x,0,board_y, tile_size);

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

		total_iterations++;
    }
	if (screen == RANDOM)
		random_screen();
	else if (screen == USER_DRAWING)
		user_drawing();
  else if (screen == MAIN_MENU)
    main_menu();
	else
		;
}
/*
void logic_gates(){
	initial_clear();
	initial_clear_chars();

	const int tile_size = 4; // 2x2
	const int board_x = RESOLUTION_X/tile_size;
	const int board_y = RESOLUTION_Y/tile_size;
	isPaused = true;

	// draw text
	char* str = "Game of Life Presets\0";
	draw_text((80-strlen(str))/2, 1,str);
	str = "- Logic gates\0";
	draw_text((80-strlen(str))/2, 3,str);
	str = "instructions blah blah\0";
	draw_text(4, 6, str);

	glider_gun(5, 10, true);

	// simulation loop
	int total_iterations = 0;
    while (1)
    {
		if (total_iterations > 0 && isPaused) // display the initial config
			continue;

        // start clearing previously drawn pixels
		deleteList(front_list);
        front_list->head = pixel_list->head;//back_list->head;
        //back_list->head = pixel_list->head;
        clear_screen(front_list,tile_size);
        pixel_list = init();
        front_list = init();
        // finish clearing previously drawn pixels

        // draw, then update the game board
        update_board_state(0,board_x,0,board_y, tile_size);

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

		total_iterations++;
    }
}
*/

/* draw, then update and compare the next board to the previous board */
void update_board_state(int x0, int x1, int y0, int y1, int size){
	int total = 0,n = RESOLUTION_X/size,m = RESOLUTION_Y/size;
    bool new_board[n][m];
    for(int i=x0;i < x1;i++){
      for(int j=y0;j < y1;j++){
        total = (game_board[(i-1)%n][(j-1)%m]+game_board[(i-1)%n][j]
        +game_board[(i-1)%n][(j+1)%m]+game_board[i][(j-1)%m]
        +game_board[i][(j+1)%m]+game_board[(i+1)%n][(j-1)%m]
        +game_board[(i+1)%n][j]+game_board[(i+1)%n][(j+1)%m])/ALIVE;

        //printf("%d %d:%d\n",i,j,total);

        if(game_board[i][j]==ALIVE){
            color_board[i][j] = rand()%65535;
            draw_tile(i*size,j*size,size,color_board[i][j]);
            insertFront(pixel_list,i,j,true);

            // cell dies if it has insufficient or too many neighbours
            if(total < 2 || total > 3){
                new_board[i][j] = DEAD;
                color_board[i][j] = 0;
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
        color_board[i][j] = 0;
			}
        }

		if (prev_board[i][j]==ALIVE && new_board[i][j]==DEAD)

		prev_board[i][j] = game_board[i][j];
      }
    }
    for(int i=x0;i < x1;i++){
      for(int j=y0;j < y1;j++){
		 game_board[i][j] = new_board[i][j];
	  }
	}
}

void initialize_board(){
    for(int i=0;i < RESOLUTION_X;i++){
      for(int j=0;j < RESOLUTION_Y;j++){
        game_board[i][j] = DEAD;
		prev_board[i][j] = DEAD;
      }
    }
    game_board[100][100] = ALIVE;
    game_board[100][101] = ALIVE;
    game_board[100][102] = ALIVE;
    game_board[100][103] = ALIVE;
    //random_initialization(0.90);
}

// 36x9 for horizontal
void glider_gun(int left_x, int top_y, bool hor_not_vert){
	if (hor_not_vert){
		block(left_x, top_y+4);
		block(left_x+34, top_y+2);
		for (int y = 0; y < 3; y++){
			game_board[left_x+10][top_y+4+y] = ALIVE;
			game_board[left_x+16][top_y+4+y] = ALIVE;
			game_board[left_x+20][top_y+2+y] = ALIVE;
			game_board[left_x+21][top_y+2+y] = ALIVE;
		}
		game_board[left_x+11][top_y+3] = ALIVE;
		game_board[left_x+11][top_y+7] = ALIVE;
		game_board[left_x+12][top_y+2] = ALIVE;
		game_board[left_x+13][top_y+2] = ALIVE;
		game_board[left_x+12][top_y+8] = ALIVE;
		game_board[left_x+13][top_y+8] = ALIVE;
		game_board[left_x+14][top_y+5] = ALIVE;
		game_board[left_x+15][top_y+3] = ALIVE;
		game_board[left_x+15][top_y+7] = ALIVE;
		game_board[left_x+17][top_y+5] = ALIVE;
		game_board[left_x+22][top_y+1] = ALIVE;
		game_board[left_x+22][top_y+5] = ALIVE;
		game_board[left_x+24][top_y] = ALIVE;
		game_board[left_x+24][top_y+1] = ALIVE;
		game_board[left_x+24][top_y+5] = ALIVE;
		game_board[left_x+24][top_y+6] = ALIVE;
	}
	else{

	}
}

void glider_eater(int left_x, int top_y, bool hor_not_vert){
}

void and_gate(int left_x, int top_y){
}
void or_gate(int left_x, int top_y){
}
void not_gate(int left_x, int top_y){
}

// spaceship 3x3
void glider (int left_x, int top_y){
	game_board[left_x][top_y+2] = ALIVE;
	game_board[left_x+1][top_y+2] = ALIVE;
	game_board[left_x+2][top_y+2] = ALIVE;
	game_board[left_x+2][top_y+1] = ALIVE;
	game_board[left_x+1][top_y] = ALIVE;
}

// 12x12
void pulsar(int centre_x, int centre_y){

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

// still-life, 3x3
void tub(int centre_x, int centre_y){
	game_board[centre_x][centre_y+1] = ALIVE;
	game_board[centre_x][centre_y-1] = ALIVE;
	game_board[centre_x+1][centre_y] = ALIVE;
	game_board[centre_x-1][centre_y] = ALIVE;
}

// still-life 2x2
void block(int left_x, int top_y){
	game_board[left_x][top_y] = ALIVE;
	game_board[left_x+1][top_y] = ALIVE;
	game_board[left_x+1][top_y+1] = ALIVE;
	game_board[left_x][top_y+1] = ALIVE;
}

// still-life 3x3
void ship(int centre_x, int centre_y){
	game_board[centre_x][centre_y+1] = ALIVE;
	game_board[centre_x][centre_y-1] = ALIVE;
	game_board[centre_x+1][centre_y] = ALIVE;
	game_board[centre_x-1][centre_y] = ALIVE;
	game_board[centre_x+1][centre_y+1] = ALIVE;
	game_board[centre_x-1][centre_y-1] = ALIVE;
}

// period 2 oscillator 3x3
void blinker(int centre_x, int centre_y){
	for (int x = centre_x-1; x < centre_x+2; x++){
		game_board[x][centre_y] = ALIVE;
	}
}

// period 2 oscillator 4x4
void toad(int left_x, int top_y){
	for (int x = left_x; x < left_x+3; x++){
		game_board[x][top_y] = ALIVE;
		game_board[x+1][top_y+1] = ALIVE;
	}
}


// 37x5
void draw_ECE243(int left_x, int top_y){
	//E
	for (int d = 0; d <=4; d++){
		game_board[left_x][top_y+d] = ALIVE;
		game_board[left_x+1][top_y+d] = ALIVE;
	}
	for (int d = 2; d<=4; d++){
		game_board[left_x+d][top_y] = ALIVE;
		game_board[left_x+d][top_y+2] = ALIVE;
		game_board[left_x+d][top_y+4] = ALIVE;
	}
	//C
	for (int d = 0; d <=4; d++){
		game_board[left_x+7][top_y+d] = ALIVE;
		game_board[left_x+8][top_y+d] = ALIVE;
	}
	for (int d = 2; d<=4; d++){
		game_board[left_x+7+d][top_y] = ALIVE;
		game_board[left_x+7+d][top_y+4] = ALIVE;
	}
	//E
	for (int d = 0; d <=4; d++){
		game_board[left_x+14][top_y+d] = ALIVE;
		game_board[left_x+15][top_y+d] = ALIVE;
	}
	for (int d = 2; d<=4; d++){
		game_board[left_x+d+14][top_y] = ALIVE;
		game_board[left_x+d+14][top_y+2] = ALIVE;
		game_board[left_x+d+14][top_y+4] = ALIVE;
	}
	//2
	for (int d = 0; d <=2; d++){
		game_board[left_x+24][top_y+d] = ALIVE;
		game_board[left_x+25][top_y+d] = ALIVE;
		game_board[left_x+21][top_y+d+2] = ALIVE;
		game_board[left_x+22][top_y+d+2] = ALIVE;
	}
	for (int d = 2; d<=4; d++){
		game_board[left_x+d+21][top_y] = ALIVE;
		game_board[left_x+d+21][top_y+2] = ALIVE;
		game_board[left_x+d+21][top_y+4] = ALIVE;
	}
	//4
	for (int d = 0; d <=2; d++){
		game_board[left_x+28][top_y+d] = ALIVE;
		game_board[left_x+29][top_y+d] = ALIVE;
		game_board[left_x+28+d][top_y+2] = ALIVE;
	}
	for (int d = 0; d <=4; d++){
		game_board[left_x+31][top_y+d] = ALIVE;
		game_board[left_x+32][top_y+d] = ALIVE;
	}
	//3
	for (int d = 0; d <=4; d++){
		game_board[left_x+37][top_y+d] = ALIVE;
		game_board[left_x+38][top_y+d] = ALIVE;
	}
	for (int d = 2; d<=4; d++){
		game_board[left_x+d+33][top_y] = ALIVE;
		game_board[left_x+d+33][top_y+2] = ALIVE;
		game_board[left_x+d+33][top_y+4] = ALIVE;
	}
}

// be careful that x1<RESOLUTION_X/size and y1<RESOLUTION_Y/size
void draw_board(int x0, int x1, int y0, int y1, int size){
	for(int i=x0;i<x1;i++){
		for(int j=y0;j<y1;j++){
			if (game_board[i][j]==ALIVE)
				draw_tile(i*size,j*size,size,tile_color);
        insertFront(pixel_list,i,j,true);
		}
	}
}

void random_initialization(int x0, int x1, int y0, int y1, float prop){
  int denom = 1000;
  srand(time(0));
  for(int i=x0;i < x1;i++){
      for(int j=y0;j < y1;j++){
          if(rand()%denom>prop*denom){
              game_board[i][j] = ALIVE;
          }
      }
  }
}

void initialize_mouse_coords(){
    // place mouse cursor in the middle of the screen
    mouse_x = RESOLUTION_X/2;
    mouse_y = RESOLUTION_Y/2;
    mouse_width = 2;
    mouse_height = 2;
}

void update_mouse_coords(int disp_x,int disp_y){

    if(disp_x>10){
      disp_x = 10;
    }else if (disp_x<-10){
      disp_x = -10;
    }
    if(disp_y>10){
      disp_y = 10;
    }else if(disp_y<-10){
      disp_y = -10;
    }

    mouse_x+=disp_x;
    mouse_y+=disp_y;

    if(mouse_x-mouse_width<0){
        mouse_x = mouse_width;
    }else if(mouse_x+mouse_width>RESOLUTION_X){
        mouse_x = RESOLUTION_X-mouse_width;
    }

    if(mouse_y-mouse_height<0){
        mouse_y = mouse_height;
    }else if(mouse_y+mouse_height>RESOLUTION_Y){
        mouse_y = RESOLUTION_Y-mouse_height;
    }
    //printf("old mouse coordinates: %d %d.\nnew mouse coordinates: %d %d",tempx,tempy,mouse_x,mouse_y);
}

void parse_lmb_click(){

    //printf("mouse loc: %d %d\n",mouse_x,mouse_y);

    // check for return to main menu
    if(screen != MAIN_MENU && mouse_bounds(7,227,82,232)){
        initialize_mouse_coords();
        screen = MAIN_MENU;
    }
}

void parse_rmb_click(){
    if(screen == USER_DRAWING && l_tile_size != 0){
        game_board[mouse_x/l_tile_size][mouse_y/l_tile_size] = ALIVE;
    }

}

void draw_tile(int x, int y, int size, short int line_color){
	for (int i = x; i < x+size; i++){
		for (int j = y; j < y+size; j++){
      if(check_bounds(i,j,1)){
        draw_pixel(i,j,line_color);
        //insertFront(pixel_list,i,j);
      }
		}
	}
}

void draw_tile_clear(int x,int y,int size,short int line_color){
  for (int i = x; i < x+size; i++){
		for (int j = y; j < y+size; j++){
      if(check_bounds(i,j,1)){
        draw_pixel(i,j,line_color);
      }
		}
	}
}

void draw_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_text(int x, int y, char * text_ptr) {
	int offset;
	volatile char * character_buffer =
	(char *)0xC9000000; // video character buffer
	/* assume that the text string fits on one line */
	offset = (y << 7) + x;
	while (*(text_ptr)) {
		*(character_buffer + offset) =
		*(text_ptr); // write to the character buffer
		++text_ptr;
		++offset;
	}
}

void draw_cursor(){
  draw_pixel(mouse_x-1,mouse_y-1,cursor_color);
  insertFront(pixel_list,mouse_x-1,mouse_y-1,false);
  draw_pixel(mouse_x+1,mouse_y-1,cursor_color);
  insertFront(pixel_list,mouse_x+1,mouse_y-1,false);
  draw_pixel(mouse_x,mouse_y,cursor_color);
  insertFront(pixel_list,mouse_x,mouse_y,false);
  draw_pixel(mouse_x-1,mouse_y+1,cursor_color);
  insertFront(pixel_list,mouse_x-1,mouse_y+1,false);
  draw_pixel(mouse_x+1,mouse_y+1,cursor_color);
  insertFront(pixel_list,mouse_x+1,mouse_y+1,false);
}

void wait_for_vsync(){
    volatile int* pixel_ctrl_ptr = (int*)0xFF203020;
    register int status;

    *pixel_ctrl_ptr = 1;

    status = *(pixel_ctrl_ptr+3);
    while((status & 0x01) != 0){
        status = *(pixel_ctrl_ptr+3);
    }
}

void initial_clear(){
    for(int i = 0;i<RESOLUTION_X;i++){
      for(int j =0;j<RESOLUTION_Y;j++){
        // clear screen by drawing everything to color of background
        	draw_pixel(i,j,bg_color);
    		  game_board[i][j] = DEAD;
      }
    }
}

// clears character buffer
void initial_clear_chars(){
	volatile char* char_ptr = (char*)0xC9000000;
	for (int j = 0; j<CHAR_MAX_Y;j++){
		for (int i = 0; i<CHAR_MAX_X; i++){
			int offset = (j<<7) + i;
			*(char_ptr+offset) = ' ';
		}
	}
}

void clear(int tile_size){
    deleteList(front_list);
    front_list->head = back_list->head;
    back_list->head = pixel_list->head;
    clear_screen(front_list,tile_size);
    pixel_list = init();
    front_list = init();
}

void clear_screen(LinkedList* list, int size){
  if(isEmpty(list)){
    return;
  }
  while(list->head!=NULL){
    // clear screen by drawing background colour over previously drawn pixels
    if(list->head->isTile){
      draw_tile_clear(list->head->x*size,list->head->y*size,size,bg_color);
    }else{
      draw_pixel(list->head->x,list->head->y,bg_color);
    }
    deleteFront(list);
  }
}

bool check_bounds(int x,int y,int size){
    return (x<RESOLUTION_X/size&&x>=0)&&(y<RESOLUTION_Y/size&&y>=0);
}

bool mouse_bounds(int x1,int y1,int x2,int y2){
    return (mouse_x<=x2&&mouse_x>=x1)&&(mouse_y<=y2&&mouse_y>=y1);
}

// set up IRQ stack pointer
void set_up_IRQ(){
	int stack, mode;
	stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes
	/* change processor to IRQ mode with interrupts disabled */
	mode = INT_DISABLE | IRQ_MODE;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r"(stack));
	/* go back to SVC mode before executing subroutine return! */
	mode = INT_DISABLE | SVC_MODE;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));

}

// enable interrupts after configuring devices
void enable_interrupts()
{
	int status = SVC_MODE | INT_ENABLE;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

// configure keys
void config_KEYs(){
	volatile int * KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
	*(KEY_ptr + 2) = 0xF; // enable interrupts for all KEYs
}

// interrupt service routine for KEYs
void KEY_ISR()
{
	volatile int * KEY_ptr = (int *)KEY_BASE;
	int press;
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // Clear the interrupt
	if ((press & 0b1) != 0) // KEY0
		isPaused ^= 1; // Toggle isPaused value
	else if ((press & 0b10) != 0) // KEY1
		screen = RANDOM;
	else if ((press & 0b100) != 0) // KEY2
		screen = USER_DRAWING;
	else if ((press & 0b1000) != 0) // KEY3
		screen = PRESETS;
	else // unexpected input
	{
		while(1)
			;
	}

	return;
}

void config_ps2(){
  volatile int * PS2_ptr = (int *)PS2_BASE;
  // PS/2 mouse needs to be reset (must be already plugged in)
  *(PS2_ptr) = 0xFF; // reset

  //enable interrupts
  *(PS2_ptr+1) = *(PS2_ptr+1)|1;

  int PS2_data = *(PS2_ptr);
  int RAVAIL = PS2_data & 0xFFFF0000;

  // clear FIFO
  while(RAVAIL>0){
    PS2_data = *(PS2_ptr);
    RAVAIL = PS2_data & 0xFFFF0000;
  }
}

void MOUSE_ISR(){
  volatile int * PS2_ptr = (int *)PS2_BASE;
  int PS2_data, RVALID, RAVAIL;
  char byte1 = 0, byte2 = 0, byte3 = 0;

  PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
  RVALID = PS2_data & 0x8000; // extract the RVALID field
  RAVAIL = PS2_data & 0xFFFF0000;

  while(RVALID || RAVAIL>0) {

    printf("\n");

    byte1 = byte2;
    byte2 = byte3;
    byte3 = PS2_data & 0xFF;

    // finished receiving 3 byte data from mouse
    if(byte1!=0){

        // check if LMB or RMB has been clicked
        if(byte1 & 1){
          printf("LMB clicked\n");
          parse_lmb_click();
        }else if(byte1 & 2){
          printf("RMB clicked\n");
          parse_rmb_click();
        }

        int x_sign = ((byte1&16)>0);
        int y_sign = ((byte1&32)>0);

        int x_movement = x_sign?-(0xFF&(~byte2+1)):(byte2);
        int y_movement = y_sign?-(0xFF&(~byte3+1)):(byte3);
        update_mouse_coords(x_movement/2,y_movement/2);
        //printf("sign bits are: %d %d\n",((byte1&16)>0),((byte1&32)>0));
        //printf("data is %d %d\n",byte2,byte3);
        //printf("test values: %d %d",)
        //printf("movement : %d %d\n",x_movement,y_movement);
    }
    //printf("bytes: %d %d %d\n",byte1,byte2,byte3);

    PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
    RVALID = PS2_data & 0x8000; // extract the RVALID field
    RAVAIL = PS2_data & 0xFFFF0000;
  }
}


// configure the GIC
void config_GIC(void)
{
	int address; // used to calculate register addresses
	/* configure the HPS timer interrupt
	*((int *)0xFFFED8C4) = 0x01000000;
	*((int *)0xFFFED118) = 0x00000080;*/
	/* configure the FPGA interval timer and KEYs interrupts */

  // setup ICDISERn so interrupts are enabled for all peripherals
	*((int *)0xFFFED108) = 0x00008300;

  // setup ICDIPTRn so interrupts target processor 1
	*((int *)0xFFFED848) = 0x00000101;
  *((int *)0xFFFED84C) = 0x01000000;

	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
	// priorities
	address = MPCORE_GIC_CPUIF + ICCPMR;
	*((int *)address) = 0xFFFF;
	// Set CPU Interface Control Register (ICCICR). Enable signaling of
	// interrupts
	address = MPCORE_GIC_CPUIF + ICCICR;
	*((int *)address) = ENABLE;
	// Configure the Distributor Control Register (ICDDCR) to send pending
	// interrupts to CPUs
	address = MPCORE_GIC_DIST + ICDDCR;
	*((int *)address) = ENABLE;
}

// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void)
{
	// Read the ICCIAR from the processor interface
	int address = MPCORE_GIC_CPUIF + ICCIAR;
	int int_ID = *((int *)address);
	switch (int_ID){
		case KEYS_IRQ: // check if interrupt is from the KEYs
			KEY_ISR();
			break;
    case PS2_IRQ:
      MOUSE_ISR();
      break;
		default:
			while (1)
			; // if unexpected, then stay here
	}
	// Write to the End of Interrupt Register (ICCEOIR)
	address = MPCORE_GIC_CPUIF + ICCEOIR;
	*((int *)address) = int_ID;
	return;
}

// Define the remaining exception handlers (equivalent to the exceptions vector table)
void __attribute__((interrupt)) __cs3_reset(void)
{
while (1)
;
}
void __attribute__((interrupt)) __cs3_isr_undef(void)
{
while (1)
;
}
void __attribute__((interrupt)) __cs3_isr_swi(void)
{
while (1)
;
}
void __attribute__((interrupt)) __cs3_isr_pabort(void)
{
while (1)
;
}
void __attribute__((interrupt)) __cs3_isr_dabort(void)
{
while (1)
;
}
void __attribute__((interrupt)) __cs3_isr_fiq(void)
{
while (1)
;
}
