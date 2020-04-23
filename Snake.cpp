//
// Created by apple on 2020/4/22.
//

#include "Snake.h"
#include <curses.h>

#define ARROW_1 27
#define ARROW_2 91
#define CH_UP 65
#define CH_DOWN 66
#define CH_RIGHT 67
#define CH_LEFT 68

Snake::Snake()
    : direction(Direction::RIGHT)
    , snake()
    , coin_position()
    , coin_expire(0)
    , coin_generate(0)
    , commands()
    , board{}
    , input_handler(nullptr)
    , score(0)
    , move_interval(250)
    , started(false)
    , again(true) {}

Snake::~Snake() {
    delete input_handler;
}

Snake::Direction Snake::Opposite(Direction dir) {
    if (dir == UP) {
        return DOWN;
    } else if (dir == DOWN) {
        return UP;
    } else if (dir == LEFT) {
        return RIGHT;
    } else {
        return LEFT;
    }
}

void Snake::Play() {
    initscr();
    noecho();
    curs_set(0);
    timeout(-1);
    srand(time(nullptr));

    while (again) {
        Initialize();

        while (MoveSnake()) {
            GenerateCoin();
            // don't know why, but the output is corrupted without this sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            refresh();
            std::this_thread::sleep_for(std::chrono::milliseconds(move_interval - 1));
        }

        Finish();
    }

    endwin();
}

void Snake::Initialize() {
    // clear the screen
    clear();
    // clear snake
    std::queue<std::pair<int, int>> temp;
    std::swap(temp, snake);
    //clear commands
    std::queue<Direction> temp2;
    std::swap(temp2, commands);
    // clear board
    for (auto & i : board) {
        for (auto & j : i) {
            j = EMPTY;
        }
    }

    // print score
    score = 0;
    UpdateScore();
    printw("\n");

    // draw board
    std::string first_line = "+" + std::string(WIDTH * 2 + 1, '-') + "+\n";
    std::string middle_line = "|" + std::string(WIDTH * 2 + 1, ' ') + "|\n";
    printw(first_line.c_str());
    for (int i = 0; i < HEIGHT; ++i) {
        printw(middle_line.c_str());
    }
    printw(first_line.c_str());

    // randomly spawn the snake
    int x = 6 + rand() % (HEIGHT - 12);
    int y = 6 + rand() % (WIDTH - 12);
    int x1 = x, x2 = x, y1 = y, y2 = y;

    int dir = rand() % 4;
    if (dir == 0) {
        direction = UP;
        x1 += 1;
        x2 += 2;
    } else if (dir == 1) {
        direction = DOWN;
        x1 -= 1;
        x2 -= 2;
    } else if (dir == 2) {
        direction = LEFT;
        y1 += 1;
        y2 += 2;
    } else {
        direction = RIGHT;
        y1 -= 1;
        y2 -= 2;
    }

    snake.emplace(x2, y2);
    snake.emplace(x1, y1);
    snake.emplace(x, y);
    board[x2][y2] = SNAKE_BODY;
    board[x1][y1] = SNAKE_BODY;
    board[x][y] = SNAKE_HEAD;
    Update(x2, y2);
    Update(x1, y1);
    Update(x, y);

    GenerateFruit();

    started = true;
    input_handler = new std::thread(&Snake::HandleInput, this);
    coin_expire = -1;
    coin_generate = COIN_GENERATE;
}

void Snake::Finish() {
    started = false;
    // update direction of the snake's head
    Update(snake.back().first, snake.back().second);

    // print instructions
    move(HEIGHT + 4, 0);
    printw("Game over. Your score: %d. Press R to retry or Q to exit.", score);
    refresh();

    // wait for input handler to finish
    input_handler->join();
    delete input_handler;
    input_handler = nullptr;
}

bool Snake::MoveSnake() {
    int x = snake.back().first;
    int y = snake.back().second;
    int newX = x;
    int newY = y;

    while (!commands.empty() && (commands.front() == direction ||
            commands.front() == Opposite(direction))) {
        commands.pop();
    }
    if (!commands.empty()) {
        direction = commands.front();
        commands.pop();
    }

    if (direction == UP) {
        if (x == 0) {
            return false;
        }
        newX = x - 1;
    } else if (direction == DOWN) {
        if (x == HEIGHT - 1) {
            return false;
        }
        newX = x + 1;
    } else if (direction == LEFT) {
        if (y == 0) {
            return false;
        }
        newY = y - 1;
    } else {
        if (y == WIDTH - 1) {
            return false;
        }
        newY = y + 1;
    }
    if (board[newX][newY] == SNAKE_BODY) {
        return false;
    }
    snake.emplace(newX, newY);
    board[x][y] = SNAKE_BODY;
    if (board[newX][newY] == FRUIT) {
        ++score;
        GenerateFruit();
    } else if (board[newX][newY] == COIN) {
        score += 5;
        coin_generate = COIN_GENERATE;
        coin_expire = -1;
    } else {
        int tail_x = snake.front().first;
        int tail_y = snake.front().second;
        board[tail_x][tail_y] = EMPTY;
        Update(tail_x, tail_y);
        snake.pop();
    }
    board[newX][newY] = SNAKE_HEAD;

    Update(x, y);
    Update(newX, newY);
    UpdateScore();
    return true;
}

void Snake::GenerateFruit() {
    int x = rand() % HEIGHT;
    int y = rand() % WIDTH;
    while (board[x][y] != EMPTY) {
        x = rand() % HEIGHT;
        y = rand() % WIDTH;
    }
    board[x][y] = FRUIT;
    Update(x, y);
}

void Snake::GenerateCoin() {
    if (coin_generate == 0) {
        int x = rand() % HEIGHT;
        int y = rand() % WIDTH;
        while (board[x][y] != EMPTY) {
            x = rand() % HEIGHT;
            y = rand() % WIDTH;
        }
        coin_position = std::pair<int, int>(x, y);
        board[x][y] = COIN;
        coin_expire += COIN_EXPIRE;
        coin_generate = -1;
        Update(coin_position.first, coin_position.second);
    } else if (coin_generate > 0 && coin_expire < 0) {
        coin_generate -= move_interval;
        if (coin_generate < 0) {
            coin_expire += coin_generate;
            coin_generate = 0;
        }
    }

    if (coin_expire == 0) {
        coin_generate += COIN_GENERATE;
        coin_expire = -1;
        board[coin_position.first][coin_position.second] = EMPTY;
        Update(coin_position.first, coin_position.second);
    } else if (coin_expire > 0) {
        coin_expire -= move_interval;
        if (coin_expire < 0) {
            coin_generate += coin_expire;
            coin_expire = 0;
        }
    }
}

void Snake::HandleInput() {
    int check = 0;
    int c = 'a';
    while (started || check != 0 || (c != 'q' && c != 'r')) {
        c = getch();
        if (check == 0 && c == ARROW_1) {
            ++check;
        } else if (check == 1 && c == ARROW_2) {
            ++check;
        } else if (check == 2) {
            if (c == CH_UP) {
                commands.push(UP);
            } else if (c == CH_DOWN) {
                commands.push(DOWN);
            } else if (c == CH_LEFT) {
                commands.push(LEFT);
            } else if (c == CH_RIGHT) {
                commands.push(RIGHT);
            } else {
                check = 0;
                continue;
            }
            check = 0;
        } else {
            check = 0;
        }
    }
    again = c == 'r';
}

void Snake::Update(int row, int col) const {
    move(row + 3, col * 2 + 2);
    if (board[row][col] == EMPTY) {
        addch(' ');
    } else if (board[row][col] == SNAKE_HEAD) {
        if (direction == UP) {
            addch('^');
        } else if (direction == DOWN) {
            addch('v');
        } else if (direction == LEFT) {
            addch('<');
        } else {
            addch('>');
        }
    } else if (board[row][col] == SNAKE_BODY) {
        addch('#');
    } else if (board[row][col] == FRUIT) {
        addch('*');
    } else if (board[row][col] == COIN) {
        addch('@');
    }
}

void Snake::UpdateScore() const {
    move(0, 0);
    printw("\n              Your score: %d", score);
}