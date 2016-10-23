#include <curses.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

WINDOW *local_win;

int cur_paddle_x = 30, cur_paddle_y = 30;
int cur_ball_x = 30, cur_ball_y = 5;

const int MIN_PADDLE_X = 3;
const int MAX_PADDLE_X = 58;
const int MIN_BALL_X = 1;
const int MAX_BALL_X = 60;
const int MIN_BALL_Y = 1;
const int MAX_BALL_Y = 30;

void update_paddle_position(int x) {
    /* Erase current paddle */
    mvaddch(cur_paddle_y, cur_paddle_x, ' ');
    mvaddch(cur_paddle_y, cur_paddle_x-1, ' ');
    mvaddch(cur_paddle_y, cur_paddle_x+1, ' ');
    mvaddch(cur_paddle_y, cur_paddle_x-2, ' ');
    mvaddch(cur_paddle_y, cur_paddle_x+2, ' ');
    /* Update to new position */
    cur_paddle_x = x;
    /* Draw new paddle */
    mvaddch(cur_paddle_y, cur_paddle_x, ACS_CKBOARD);
    mvaddch(cur_paddle_y, cur_paddle_x-1, ACS_CKBOARD);
    mvaddch(cur_paddle_y, cur_paddle_x+1, ACS_CKBOARD);
    mvaddch(cur_paddle_y, cur_paddle_x-2, ACS_CKBOARD);
    mvaddch(cur_paddle_y, cur_paddle_x+2, ACS_CKBOARD);
}

void update_ball_position(int x, int y) {
    /* Erase current ball */
    mvaddch(cur_ball_y, cur_ball_x, ' ');
    /* Update to new position */
    cur_ball_x = x;
    cur_ball_y = y;
    /* Draw new ball */
    mvaddch(cur_ball_y, cur_ball_x, 'o');
}

int main(int argc, char** argv) {
	initscr();			/* Start curses mode */
    refresh();
    
    /* Create window for game walls */
	local_win = newwin(32, 62, 0, 0);
	box(local_win, 0 , 0);
    /* Create initial paddle and ball */
    update_paddle_position(cur_paddle_x);
    update_ball_position(cur_ball_x, cur_ball_y);
    /* Display starting board */
    wrefresh(local_win);
    
    /* Diable cursor and echoing */
    curs_set(0);
    noecho();
	
    /* Get input */
    char c;
    char str[6];
    int new_x, new_y;
    while(c = getch()) {
        switch(c) {
          case 'b':
            getnstr(str, 6);
            str[2] = '\0';
            str[5] = '\0';
            new_x = atoi(&str[0]);
            new_y = atoi(&str[3]);
            
            if(MIN_BALL_X <= new_x && new_x <= MAX_BALL_X && MIN_BALL_Y <= new_y && new_y <= MAX_BALL_Y) {
                update_ball_position(new_x, new_y);
            }
            break;
          case 'p':
            getnstr(str, 3);
            str[2] = '\0';
            new_x = atoi(&str[0]);
            if(MIN_PADDLE_X <= new_x && new_x <= MAX_PADDLE_X) {
                update_paddle_position(new_x);
            }
            break;
          case 'q':
            goto out;
            break;
          default:
            break;
        }
        
        /* After processing input, update screen */
        wrefresh(local_win);
        refresh();
    }

out:
    /* Teardown and exit */
	endwin();

	return 0;
}
