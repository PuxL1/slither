#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

typedef struct strct
{
    int y;
    int x;
    struct strct *next;

} cords;

void make_body(cords *now,int y, int x, bool make,WINDOW *win);
void show_body(cords *head);
void apple_found(int *score, cords *apple,WINDOW *win);
void death_screen(WINDOW *win, cords *head);
void ch_pressed(cords *mov, WINDOW *win, bool *mov_status);

char head_ch = 'N';

int main(void) 
{
    srand(time(NULL));

    cords mov = {0,0,NULL};

    int row, col;
    row = (rand() % 23) + 2;
    col = (rand() % 100) + 2;

    WINDOW *win = initscr();
    cords head = {getmaxy(win)/2,getmaxx(win)/2,NULL};
    cords apple = {row,col,NULL};

    int score = 0;
    bool mov_status;

    /*настройки*/

    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    noecho();
    keypad(win,true);
    nodelay(win,1);
    curs_set(false);

    /*настройки*/
    
    while (true) {
        ch_pressed(&mov,win,&mov_status);
        if (head.y == apple.y && head.x == apple.x) {
            apple_found(&score, &apple,win);
            make_body(&head, head.y+mov.y, head.x+mov.x, true, win);
        }

        clear();
        attron(COLOR_PAIR(3));
        box(win,'|','-');
        mvwprintw(stdscr, 0, getmaxx(win)/2-3, "score = %d", score);
        if (!(head.y >  0 && head.y < getmaxy(win)-1) || !(head.x > 0 && head.x < getmaxx(win)-1)) 
            death_screen(win, &head);

        attron(COLOR_PAIR(1));
        mvaddch(apple.y,apple.x,'@');
        make_body(&head, head.y+mov.y, head.x+mov.x, false, win);
        show_body(&head);
        usleep((mov_status) ? 125000 : 125000/2);
    }

    endwin(); 
    return 0;
}

void apple_found(int *score, cords *apple,WINDOW *win)
{
    srand(time(NULL));
    (*score)++;
    apple->y = 2 + rand() % (getmaxy(win) - 13);
    apple->x = 3 + rand() % (getmaxx(win) - 15);
}

void make_body(cords *now, int new_y, int new_x, bool make, WINDOW *win)
{
    int old_x = now->x;
    int old_y = now->y;
    now->x = new_x;
    now->y = new_y;

    cords *next, *last;
    for (last = now, next = now->next; next != NULL ; last = next, next = next->next) {
        new_x = next->x;
        new_y = next->y;
        next->x = old_x;
        next->y = old_y;
        old_x = new_x;
        old_y = new_y;
        if (now->x == old_x && now->y == old_y)
            death_screen(win, now);
    }

    if (make) {
        cords *new = malloc(sizeof(cords));
        last->next = new;
        new->next = NULL;
        new->y = new_y;
        new->x = new_x;
    }
}


void death_screen(WINDOW *win, cords *head)
{
    attron(COLOR_PAIR(1));
    clear();
    mvwprintw(stdscr,getmaxy(win)/2-1,getmaxx(win)/2-6,"-GAMEOVER-");
    box(win,'|','-');
    show_body(head);
    refresh();
    sleep(2);
    nodelay(win,0);
    wgetch(win);
    endwin();
    exit(0);
}

void show_body(cords *head)
{
    attron(COLOR_PAIR(2));
    mvaddch(head->y,head->x,head_ch);

    for (cords *temp = head->next; temp != NULL; temp = temp->next)
        mvaddch(temp->y,temp->x,'o');
}

void ch_pressed(cords *mov, WINDOW *win, bool *mov_status)
{
    int pressed = wgetch(win);
    switch (pressed)
    {
        case 'w':
                if (!(*mov_status)) {
                mov->y = -1;
                mov->x = 0;
                head_ch = '^';
            }
            *mov_status = true; 
            break;
        case 's':
                if (!(*mov_status)) {
                mov->y = 1;
                mov->x = 0;
                head_ch = 'v';
            }
            *mov_status = true; 
            break;
        case 'd':
                if (*mov_status) {
                mov->y = 0;
                mov->x = 1;
                head_ch = '>';
            }
            *mov_status = false;
            break; 
        case 'a':
                if (*mov_status) {
                mov->y = 0;
                mov->x = -1;
                head_ch = '<';
            }
            *mov_status = false;
            break;
        case '\e':
            endwin();
            exit(0);
            break;
        default:
            break;
    }

}
