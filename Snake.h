//
// Created by apple on 2020/4/22.
//

#ifndef SNAKE_PRACTICE_SNAKE_H
#define SNAKE_PRACTICE_SNAKE_H

#include <queue>
#include <unordered_map>
#include <thread>

class Snake {
public:
    Snake();

    ~Snake();

    void Play();

private:
    enum Item {
        EMPTY,
        SNAKE_HEAD,
        SNAKE_BODY,
        FRUIT,
        COIN
    };

    enum Direction {
        UP,
        DOWN,
        RIGHT,
        LEFT
    };

    static Direction Opposite(Direction dir);

    void Initialize();

    void Finish();

    bool MoveSnake();

    void GenerateFruit();

    void GenerateCoin();

    void HandleInput();

    void UpdateScore() const;

    void Update(int row, int col) const;

private:
    static const int WIDTH = 20, HEIGHT = 20;
    static const int COIN_EXPIRE = 15000, COIN_GENERATE = 10000;

    Direction direction;

    std::queue<std::pair<int, int>> snake;

    int coin_expire;

    int coin_generate;

    std::pair<int, int> coin_position;

    Item board[WIDTH][HEIGHT];

    std::queue<Direction> commands;

    std::thread* input_handler;

    int score;

    // milliseconds
    int move_interval;

    bool started;

    bool again;
};


#endif //SNAKE_PRACTICE_SNAKE_H
