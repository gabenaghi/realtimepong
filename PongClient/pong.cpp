#include "mbed.h"
#include "rtos.h"

//these are likely wrong. change experimentally.
#define PADDLE_MAX 60
#define PADDLE_MIN 0
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 30
#define PADDLE_HEIGHT 5

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
bool didMove = false;
bool paddir = false;

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
    //pc.printf("move_right");

    if ( paddle_pos <  PADDLE_MAX){
        paddle_pos ++;
    }
    didMove = true;
    paddir = true;

}
void paddleMoved(bool whichWay, bool moved ){
    if(whichWay==false && moved ==true){
        pc.printf("leftMoved\n");
    }
    if(whichWay==true && moved ==true){
        pc.printf("rightMoved\n");
    }
    didMove = false;
}

void move_paddle_left()
{
    //pc.printf("move_left");
    if ( paddle_pos > PADDLE_MIN ){
        paddle_pos --;
    }
    didMove = true;
    paddir = false;
}

void update_board()
{
    report_paddle();
    report_ball();
}

void collision(){
        ball_x_vel++;
        ball_y_vel++;
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



Thread ball_thread(osPriorityNormal);
Thread screen_thread(osPriorityNormal);

void signal_refresh()
{
     screen_thread.signal_set(0x01);
}

void screen_update_thread()
{
        RtosTimer screen_refresh_timer(signal_refresh);
        screen_refresh_timer.start(50);
        while (1)
        {
            Thread::signal_wait(0x01);
            update_board();
        }

}


int main()
{
    screen_thread.start(screen_update_thread);

    // attach isrs to the interrupt pins
    right_button.rise(move_paddle_right);
    left_button.rise(move_paddle_left);

    //initialize ball and paddle positions
    ball_posx = BOARD_WIDTH / 2;
    ball_posy = BOARD_HEIGHT / 2;
    paddle_pos = BOARD_WIDTH / 2;

    //generate random ball angle
    //TODO: think of a better random vel function
    ball_x_vel = rand()%2+1;
    ball_y_vel = rand()%2+1;



    ball_thread.start(ball_move);
    while(1){
        paddleMoved(paddir,didMove);

    };
}
