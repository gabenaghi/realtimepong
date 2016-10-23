#include "mbed.h"
#include "rtos.h"

//these are likely wrong. change experimentally.
#define PADDLE_MAX 60
#define PADDLE_MIN 0
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 30
#define PADDLE_HEIGHT 1
#define BALL_UP true
#define BALL_RIGHT true
#define BALL_LEFT false
#define BALL_DOWN false

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
Mutex padlock;
//Threads
Thread ball_thread(osPriorityNormal);
Thread screen_thread(osPriorityNormal);

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
    if ( paddle_pos <  PADDLE_MAX){
        paddle_pos ++;
    }
}

void move_paddle_left()
{
    //pc.printf("move_left");
    if ( paddle_pos > PADDLE_MIN ){
        paddle_pos--;
    }

}

void update_board()
{
    //padlock.lock();
    report_paddle();
    report_ball();
    //padlock.unlock();
    ball_thread.signal_set(0x01);
}

void update_ball(){
        ball_posx += ball_x_vel;
        ball_posy += ball_y_vel;
}
//makes proper adjustment for collisions for neg. & pos. numbers
void collision(){
        ball_x_vel = (ball_x_vel<=0)? (ball_x_vel--):(ball_x_vel++);
        ball_y_vel = (ball_y_vel<=0)? (ball_y_vel--):(ball_y_vel++);
    }

void ball_move()
{ while(1){
        //Waits on screen thread to say that its drawn the board before proceeding
        Thread::signal_wait(0x01);
        //collision check
        //padlock.lock();
        if((ball_posy == (BOARD_HEIGHT-PADDLE_HEIGHT)) && (ball_posx>= (paddle_pos- 2))&& (ball_posx<= (paddle_pos+2))){
            collision();
            ball_posy = BOARD_HEIGHT-PADDLE_HEIGHT -1;
            ball_y_vel*=-1;
            update_ball();
            //break;
        }
        //padlock.unlock();

        //hits right wall, reverse x direction
        if (ball_posx > BOARD_WIDTH){
            ball_posx = BOARD_WIDTH - (ball_posx - BOARD_WIDTH);
            ball_x_vel = -1 * ball_x_vel;

        }
        //hits left wall, reverse x direction
        if (ball_posx <= 0 ){
            ball_posx = -1 * ball_posx;
            ball_x_vel = -1 * ball_x_vel;

        }
        //hit top, reverse direction y direction
        if (ball_posy < 0 ){
            ball_posy = -1 * ball_posy;
            ball_y_vel = -1 * ball_y_vel;

        }

        update_ball();
         //lost game
        if (ball_posy > BOARD_HEIGHT-PADDLE_HEIGHT){
            pc.printf("q\n");
        }
    }
}





void signal_refresh()
{
     screen_thread.signal_set(0x01);
}

void screen_update_thread()
{
        RtosTimer screen_refresh_timer(signal_refresh);
        /*Gabe: changed to 150 to check if time to print was affecting the
         *speed with which the ball and paddle could realize they are in the same place.
         *Results were inconclusive.
         */
        screen_refresh_timer.start(150);
        while (1)
        {
            Thread::signal_wait(0x01);
            update_board();
        }

}


int main()
{
      //gives you five seconds to start the game after resetting mbed

     Thread::wait(5000);


    // attach isrs to the interrupt pins
    right_button.rise(move_paddle_right);
    left_button.rise(move_paddle_left);

    //initialize ball and paddle positions
    ball_posx = BOARD_WIDTH / 2;
    ball_posy = BOARD_HEIGHT-5;
    paddle_pos = BOARD_WIDTH / 2;

    //generate random ball angle
    //TODO: think of a better random vel function
    ball_x_vel = 1;
    ball_y_vel = -1;


    ball_thread.start(ball_move);
    screen_thread.start(screen_update_thread);
    while(1){
        //paddleMoved(paddir,didMove);

    };
}
