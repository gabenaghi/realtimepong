#include "mbed.h"
#include "rtos.h"

//these are likely wrong. change experimentally.
#define PADDLE_MAX 60
#define PADDLE_MIN 0
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 30

#define INITIAL_BALL_TIME 0.5f

#define FPS 20.0f
#define REFRSH_TIME 1/FPS


//globals
Serial pc(USBTX, USBRX);
InterruptIn right_button(p5);
InterruptIn left_button(p6);
int ball_posx, ball_posy;
int paddle_pos;
int ball_x_vel, ball_y_vel;

float ball_pause_time = INITIAL_BALL_TIME;

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

void update_board()
{
    report_paddle();
    report_ball();   
}

void ball_move()
{
    //increment x position
    ball_posx += ball_x_vel;
    ball_posx += ball_y_vel;

    //check and adjust if x out of bounds
    if (ball_posx > BOARD_WIDTH){
        ball_posx = BOARD_WIDTH - (ball_posx - BOARD_WIDTH);
        ball_x_vel = -1 * ball_x_vel;
    }
    if (ball_posx < 0 ){
        ball_posx = -1 * ball_posx;
        ball_x_vel = -1 * ball_x_vel;
    }

    //check and adjust if y out of bounds
    if (ball_posy > BOARD_HEIGHT){
        ball_posy = BOARD_HEIGHT - (ball_posy - BOARD_HEIGHT);
        ball_y_vel = -1 * ball_y_vel;
    }
    if (ball_posy < 0 ){
        ball_posy = -1 * ball_posy;
        ball_y_vel = -1 * ball_y_vel;
    }

}

Thread ball_thread(osPriorityNormal);

int main()
{ 
    // attach isrs to the interrupt pins    
    right_button.rise(&move_paddle_right);
    left_button.rise(&move_paddle_left);

    //initialize ball and paddle positions
    ball_posx = BOARD_WIDTH / 2;
    ball_posy = BOARD_HEIGHT / 2;
    paddle_pos = BOARD_WIDTH / 2;
    
    //generate random ball angle
    //TODO: think of a better random vel function
    ball_x_vel = rand()%2+1;
    ball_y_vel = rand()%2+1;
    
    // draw the initial positions on the board
    report_paddle();
    report_ball();

    
    RtosTimer screen_refresh_timer(update_board);

    screen_refresh_timer.start(50);
    ball_thread.start(ball_move);
    
    while(1);
}