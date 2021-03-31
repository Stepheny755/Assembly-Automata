
/* De1-SoC FPGA defines */
#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

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

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

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

volatile int pixel_buffer_start; // global variable

/* LinkedList definition start */
typedef struct node{
  int x;
  int y;
  struct node* next;
}Node;

typedef struct linkedlist{
  Node* head;
}LinkedList;

Node* createNode(int x,int y){
  Node* temp = (Node*)malloc(sizeof(Node));
  temp->x = x;
  temp->y = y;
  temp->next = NULL;
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

void insertFront(LinkedList* list,int x,int y){
  Node* temp = createNode(x,y);
  temp->next = list->head;
  list->head = temp;
}

void insertTail(LinkedList* list,int x,int y){
  if(isEmpty(list)){
    list->head=createNode(x,y);
    return;
  }
  Node* temp = list->head;
  while(temp->next!=NULL){
    temp=temp->next;
  }
  temp->next = createNode(x,y);
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

// global drawn pixels linked lists
LinkedList* pixel_list;
LinkedList* front_list;
LinkedList* back_list;

// global colour values
short int bg_color;
short int tile_color;

// game board
int game_board[RESOLUTION_X][RESOLUTION_Y];

/* GLOBAL VARIABLES END */


/* GAME FUNCTIONS */

// set the starting board state
void initialize_board();

// initialize the board randomly with prop % chance that a cell is alive at the start
void random_initialization(float prop);

// update and draw the board
void update_board_state();

/* GAME FUNCTIONS END */


/* HELPER FUNCTIONS */

// plot a pixel with given colour at coordinates
void draw_pixel(int x, int y, short int line_color);

// draw a line with given colour between coordinates
void draw_line(int ax,int ay,int bx,int by,short int line_color);

// draw a square with given width/height at the coordinates
void draw_rect(int x,int y,int width, int height,short int color);

// wait for vertical synchronization
void wait_for_vsync();

// clear the screen on startup (slow clear)
void initial_clear();

// clear the screen between frames (fast clear);
void clear_screen(LinkedList* list);

// check if coordinates is on game board
bool check_bounds(int x,int y);

/* HELPER FUNCTIONS END */


int main(void)
{
    // set background color for clearing screen
    bg_color = WHITE;

    // set tile color for pixels
    tile_color = BLACK;

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */

    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    initial_clear();

    pixel_list = init();
    front_list = init();
    back_list = init();

    //srand(time(NULL));

     // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    initial_clear();

    // initialize the game board
    initialize_board();

    while (1)
    {
        // start clearing previously drawn pixels
        front_list->head = back_list->head;
        back_list->head = pixel_list->head;
        pixel_list = init();
        clear_screen(front_list);
        // finish clearing previously drawn pixels


        // update and draw the game board
        update_board_state();

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }

}

void initialize_board(){
    for(int i=0;i < RESOLUTION_X;i++){
      for(int j=0;j < RESOLUTION_Y;j++){
        game_board[i][j] = DEAD;
      }
    }
    game_board[100][100] = ALIVE;
    game_board[100][101] = ALIVE;
    game_board[100][102] = ALIVE;
    game_board[100][103] = ALIVE;
    random_initialization(0.01);
}

void random_initialization(float prop){

  int denom = 1000;
  for(int i=0;i < RESOLUTION_X;i++){
      for(int j=0;j < RESOLUTION_Y;j++){
          if(rand()%denom>prop*denom){
              game_board[i][j] = ALIVE;
          }
      }
  }
}

void update_board_state(){

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
                game_board[i][j] = DEAD;
            }else{
                draw_pixel(i,j,tile_color);
            }
        }else{
            // cell lives if it has 3 neighbours
            if(total==3){
                game_board[i][j] = ALIVE;
                draw_pixel(i,j,tile_color);
            }
        }
      }
    }
}

void draw_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_line(int ax,int ay,int bx,int by,short int line_color){

    int dx=ABS(bx-ax);
    int dy=-ABS(by-ay);
    int sx=(ax<bx)?1:-1;
    int sy=(ay<by)?1:-1;
    int error = dx+dy, err2;

    for(int x = ax,y = ay;!(x==bx&&y==by);){
      draw_pixel(x,y,line_color);
      insertFront(pixel_list,x,y);
      err2 = 2*error;
      if(err2>=dy){
        x+=sx;
        error+=dy;
      }
      if(err2<=dx){
        y+=sy;
        error+=dx;
      }
    }
}

void draw_rect(int x,int y,int width, int height,short int color){
    for(int i = -width;i<width;i++){
       for(int j = -width;j<height;j++){
          draw_pixel(x+i,y+j,color);
          insertFront(pixel_list,x+i,y+j);
       }
    }
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
      }
    }
}

void clear_screen(LinkedList* list){
  if(isEmpty(list)){
    return;
  }
  while(list->head!=NULL){
    // clear screen by drawing background colour over previously drawn pixels
    draw_pixel(list->head->x,list->head->y,bg_color);
    deleteFront(list);
  }
}

bool check_bounds(int x,int y){
    return (x<RESOLUTION_X&&x>=0)&&(y<RESOLUTION_Y&&y>=0);
}
