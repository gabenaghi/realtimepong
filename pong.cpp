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

#define BALL_MIN BOARD_HEIGHT - 1


//globals
Serial pc(USBTX, USBRX);
InterruptIn right_button(p5);
InterruptIn left_button(p6);
int ball_posx, ball_posy;
int paddle_pos;
int ball_x_vel, ball_y_vel;

//Threads
Thread ball_thread(osPriorityNormal);
Thread screen_thread(osPriorityNormal);

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
    if ( paddle_pos > PADDLE_MIN ){
        paddle_pos--;
    }

}

void update_board()
{
    report_paddle();
    report_ball();
    ball_thread.signal_set(0x01);
}

void update_ball(){
        ball_posx += ball_x_vel;
        ball_posy += ball_y_vel;
}
//makes proper adjustment for collisions for neg. & pos. numbers
void collision()
{
        ball_x_vel = (ball_x_vel<=0)? (ball_x_vel--):(ball_x_vel++);
        ball_y_vel = (ball_y_vel<=0)? (ball_y_vel--):(ball_y_vel++);
}

void ball_move()
{
    while(1)
    {
        //Waits on screen thread to say that its drawn the board before proceeding
        Thread::signal_wait(0x01);
        //collision check
        //padlock.lock();
        update_ball();

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

        //came from in bounds and now out of bounds
        if((ball_posy - ball_y_vel < BALL_MIN) && (ball_posy >= BALL_MIN)) //now out of bounds
        {
            if ( (ball_posx >= paddle_pos - 2) && (ball_posx <= (paddle_pos + 2)))
            {
                collision();
                ball_posy = BALL_MIN - (ball_posy - BALL_MIN );
                ball_y_vel*=-1;
            }
            else
            {
                //lost game
                pc.printf("q\n");

                //reset
                //initialize ball and paddle positions
                ball_posx = BOARD_WIDTH / 2;
                ball_posy = BOARD_HEIGHT / 2;
                paddle_pos = BOARD_WIDTH / 2;

                //generate random ball angle
                ball_x_vel = (rand() % 2 == 0) ? 1 : -1;
                ball_y_vel = (rand() % 2 == 0) ? 1 : -1;
                Thread::wait(5000);
            }


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
    Thread::wait(1000);

    // attach isrs to the interrupt pins
    right_button.rise(move_paddle_right);
    left_button.rise(move_paddle_left);

    //initialize ball and paddle positions
    ball_posx = BOARD_WIDTH / 2;
    ball_posy = BOARD_HEIGHT / 2;
    paddle_pos = BOARD_WIDTH / 2;

    //generate random ball angle
    ball_x_vel = (rand() % 2 == 0) ? 1 : -1;
    ball_y_vel = (rand() % 2 == 0) ? 1 : -1;

    ball_thread.start(ball_move);
    screen_thread.start(screen_update_thread);
    while(1){
        //sleep forever
        Thread::signal_wait(0x04);
    };
}
