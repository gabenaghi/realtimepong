#include "mbed.h"
#include "rtos.h"

//these are likely wrong. change experimentally.
#define PADDLE_MAX 60
#define PADDLE_MIN 0
#define BOARD_WIDTH 60
#define BOARD_HIGHT 30

//globals
Serial pc(USBTX, USBRX);
InterruptIn right_button(p20);
InterruptIn left_button(p19);
int ball_posx, ball_posy;
int paddle_pos;
int ball_x_vel, ball_y_vel;

void report_paddle()
{
    pc.printf("p%02d\n", paddle_pos);
}

void report_ball()
{
    pc.printf("b%02d,%02d\n", ball_posx, ball_posy);
}

void move_paddle_right()
{
    if ( paddle_pos <  PADDLE_MAX)
        paddle_pos ++;
}

void move_paddle_left()
{
    if ( paddle_pos > PADDLE_MIN )
        paddle_pos --;
}

int main()
{
    //initialize ball and paddle positions
    ball_posx = BOARD_WIDTH / 2;
    ball_posy = BOARD_HIGHT / 2;
    paddle_pos = BOARD_WIDTH / 2;
    
    //generate random ball angle
    //TODO: think of a better random vel function
    ball_x_vel = rand()%2+1;
    ball_y_vel = rand()%2+1;
    
    // draw the initial positions on the board
    report_paddle();
    report_ball();
    
    // attach isrs to the interrupt pins    
    right_button.rise(move_paddle_right);
    left_button.rise(move_paddle_left);
    
    
    while(1);
}