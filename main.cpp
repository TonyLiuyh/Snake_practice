#include <iostream>
#include <curses.h>
#include "Snake.h"
#define ARROW_1 27
#define ARROW_2 91
#define UP 65
#define DOWN 66
#define RIGHT 67
#define LEFT 68

int main() {

    Snake snake;
    snake.Play();

    return 0;
}
