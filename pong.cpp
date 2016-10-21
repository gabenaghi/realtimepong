#include "mbed.h"
#include "rtos.h"

//these are likely wrong. change experimentally.

#define BOARD_WIDTH 60
#define BOARD_HEIGHT 30

#define PADDLE_HEIGHT 5
#define PADDLE_WIDTH 6
#define PADDLE_MAX BOARD_WIDTH - PADDLE_WIDTH
#define PADDLE_MIN 0

#define BALL_SIDE_LENGTH 5
#define BALL_SIGNAL 0x02

#define SCREEN_REFRSH_MS 100
#define SCREEN_REFRESH_SIGNAL 0x01

#define BALL_MOVE_MS 200


//globals
Serial pc(USBTX, USBRX);
InterruptIn right_button(p5);
InterruptIn left_button(p6);
int ball_posx, ball_posy;
int paddle_pos;
int ball_x_vel, ball_y_vel;
Thread screen_thread(osPriorityNormal);
Thread ball_thread(osPriorityNormal);

/*
bool didMove = false;
bool paddir = false;


void paddleMoved(bool whichWay, bool moved ){
    if(whichWay==false && moved ==true){
        pc.printf("leftMoved\n");
    }
    if(whichWay==true && moved ==true){
        pc.printf("rightMoved\n");
    }
    didMove = false;
}
*/

/**********************/
/*Screen Refresh Stuff*/
/**********************/

void report_paddle()
{
    pc.printf("p%02d\n", paddle_pos);
}

void report_ball()
{
    pc.printf("b%02d,%02d\n", ball_posx, ball_posy);
}

void update_board()
{
    report_paddle();
    report_ball();   
}

void signal_refresh()
{
     screen_thread.signal_set(SCREEN_REFRESH_SIGNAL);
}

void screen_update_thread()
{
        RtosTimer screen_refresh_timer(signal_refresh);
        screen_refresh_timer.start(SCREEN_REFRSH_MS);
        while (1)
        {
            Thread::signal_wait(SCREEN_REFRESH_SIGNAL);
            update_board();
        }
    
}

/***********************/
/*   Ball Move Logic   */
/***********************/

void signal_ball_move()
{
    ball_thread.signal_set(BALL_SIGNAL);
}

void collision()
{
    ball_x_vel++;
    ball_y_vel++;
}

void ball_move()
{
    RtosTimer ball_move_timer(signal_ball_move);
    ball_move_timer.start(BALL_MOVE_MS);

    while (1)
    {
        Thread::signal_wait(BALL_SIGNAL);

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
        //also the case for when you lose the game
        if (ball_posy > BOARD_HEIGHT-PADDLE_HEIGHT){
            ball_posy = BOARD_HEIGHT - (ball_posy - BOARD_HEIGHT);
            ball_y_vel = -1 * ball_y_vel;
        }
        if (ball_posy < 0 ){
            ball_posy = -1 * ball_posy;
            ball_y_vel = -1 * ball_y_vel;
        }
    }
}

/***********************/
/*   Push Button ISRs  */
/***********************/
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
    // low priority thread manages screen refreshes @ constant rate
    screen_thread.start(screen_update_thread);

    //initialize ball and paddle positions
    ball_posx = BOARD_WIDTH / 2;
    ball_posy = BOARD_HEIGHT / 2;
    paddle_pos = BOARD_WIDTH / 2;

    //this is garbage
    ball_x_vel = 1;
    ball_y_vel = 1;
    /*  
    //generate random ball angle
    //TODO: think of a better random vel function
    ball_x_vel = rand()%2+1;
    ball_y_vel = rand()%2+1;
    */

    ball_thread.start(ball_move);

    // attach isrs to the interrupt pins    
    right_button.rise(move_paddle_right);
    left_button.rise(move_paddle_left);


    while(1){
        // should sleep forever
        Thread::signal_wait(0);
    }
}