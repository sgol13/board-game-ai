#include <ctime>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <unistd.h>
#include <utility>

using namespace std;

// display current board state
void displayBoard(const int board[8][8]) {

    cout << "\033[2J\033[1;1H";
    cout << "\n\n";

    for (int y = 7; y >= 0; y--) {
        cout << "    " << (char)('A' + y) << " |";
        for (int x = 0; x <= 7; x++) {

            char c = ' ';
            if (board[x][y] == 1)
                c = '#';
            else if (board[x][y] == 2) {
                cout << "\033[1;34m";
                c = 'A';
            } else if (board[x][y] == 3) {
                c = 'B';
                cout << "\033[1;31m";
            }

            cout << c;
            cout << "\033[0m";
            cout << "|";
        }
        cout << endl;
    }

    cout << "       ";
    for (int x = 0; x <= 7; x++)
        cout << x << " ";
    cout << endl;
}

// creates new board
void createBoard(int board[8][8]) {

    // black_pawns pola
    board[0][3] = board[1][1] = board[1][6] = 1;
    board[2][4] = board[3][2] = board[3][7] = 1;
    board[4][0] = board[4][5] = board[5][3] = 1;
    board[6][1] = board[6][6] = board[7][4] = 1;

    board[5][0] = board[6][0] = board[7][0] = 2; // piony B
    board[0][7] = board[1][7] = board[2][7] = 3; // piony C
}

// changes column's char to int
int columnToNumber(char x) {

    if (x >= 'a' && x <= 'h') // letters 'a' - 'h'
        return x - 'a';
    else if (x >= '0' && x <= '7') // digits '0' - '7'
        return x - '0';
    else
        return 0; // error if sth else
}

// checks if column's / row's number is in a correct range
bool checkRange(int x) { return x >= 0 && x <= 7; }

// reads input and checks if it's correct
bool readInput(int move[4]) {

    string input;
    getline(cin, input);

    // omit white characters
    int r = 0;
    while (input[r] == '\t' || input[r] == ' ')
        ++r;

    bool correct_format = true;

    // initial row
    if (isalpha(input[r]) && checkRange(columnToNumber(input[r])))
        move[1] = columnToNumber(input[r++]);
    else
        correct_format = false;

    // initial column
    if (isdigit(input[r]) && checkRange(columnToNumber(input[r])))
        move[0] = columnToNumber(input[r++]);
    else
        correct_format = false;

    // omit white characters
    while (input[r] == '\t' || input[r] == ' ')
        ++r;

    // final row
    if (isalpha(input[r]) && checkRange(columnToNumber(input[r])))
        move[3] = columnToNumber(input[r++]);
    else
        correct_format = false;

    // final column
    if (isdigit(input[r]) && checkRange(columnToNumber(input[r])))
        move[2] = columnToNumber(input[r++]);
    else
        correct_format = false;

    while (input[r]) {
        if (input[r] != ' ' && input[r] != '\t' && input[r] != '\n')
            correct_format = false;
        ++r;
    }

    return correct_format;
}

// chcks if the given move follows the game rules
bool isCorrectMove(const int move[4], int board[8][8]) {

    int x1 = move[0];
    int y1 = move[1];
    int x2 = move[2];
    int y2 = move[3];
    int dx = 0, dy = 0;

    if (x1 == x2 && y1 == y2)
        return false;

    if (board[x1][y1] != 2)
        return false;

    if (y1 == y2) {
        if (x1 < x2)
            dx = 1;
        else
            dx = -1;
    }

    if (x1 == x2) {
        if (y1 < y2)
            dy = 1;
        else
            dy = -1;
    }

    if (x1 + y1 == x2 + y2) {
        if (x1 < x2) {
            dx = 1;
            dy = -1;
        } else {
            dx = -1;
            dy = 1;
        }
    }

    if (x1 - y1 == x2 - y2) {
        if (x1 < x2)
            dx = dy = 1;
        else
            dx = dy = -1;
    }

    if (dx == 0 && dy == 0)
        return false;

    do {
        x1 += dx;
        y1 += dy;

        if (board[x1][y1] != 0)
            return false;

    } while (x1 != x2 || y1 != y2);

    return true;
}

// updates pawns' positions
void executeMove(const int move[4], int pawns[3][2], int board[8][8],
                 const int player) {

    board[move[0]][move[1]] = 0;
    board[move[2]][move[3]] = player;

    for (int i = 0; i < 3; i++) {
        if (move[0] == pawns[i][0] && move[1] == pawns[i][1]) {
            pawns[i][0] = move[2];
            pawns[i][1] = move[3];
            break;
        }
    }
}

// calculates the assessment vaulue of the current state for the given player
int calculateStateValue(const int board[8][8], const int pawns[3][2],
                        const int player) {

    int state_value = 0;
    const int direction[8][2] = {{0, 1}, {1, 0},  {0, -1},  {-1, 0},
                                 {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    const int final_positions[2][3][2] = {{{0, 7}, {1, 7}, {2, 7}},
                                          {{5, 0}, {6, 0}, {7, 0}}};

    for (int pawn = 0; pawn < 3; pawn++) {

        int d[8][8]{};

        int pawn_x = pawns[pawn][0];
        int pawn_y = pawns[pawn][1];

        queue<pair<int, int>> Q;
        Q.push(make_pair(pawn_x, pawn_y));
        d[pawn_x][pawn_y] = 1;

        while (!Q.empty()) {

            pawn_x = Q.front().first;
            pawn_y = Q.front().second;
            Q.pop();

            for (int direction_num = 0; direction_num < 8; direction_num++) {

                int x = pawn_x;
                int y = pawn_y;
                int dx = direction[direction_num][0];
                int dy = direction[direction_num][1];
                while (
                    checkRange(x + dx) && checkRange(y + dy) &&
                    (board[x + dx][y + dy] == 0 || board[x + dx][y + dy] == player)) {
                    x += dx;
                    y += dy;

                    if (d[x][y] == 0) {
                        d[x][y] = d[pawn_x][pawn_y] + 1;
                        Q.push(make_pair(x, y));
                    }
                }
            }
        }

        int shortest_path = 10;

        for (int i = 0; i < 3; i++) {

            int final_position_x = final_positions[player - 2][i][0];
            int final_position_y = final_positions[player - 2][i][1];

            if (pawns[pawn][0] == final_position_x &&
                pawns[pawn][1] == final_position_y)
                state_value -= 100;

            if (d[final_position_x][final_position_y] != 0 &&
                d[final_position_x][final_position_y] < shortest_path)
                shortest_path = d[final_position_x][final_position_y];
        }

        // powieksz wartosc obecnego stanu
        state_value += shortest_path;
    }

    return state_value;
}

// finds an optimal move for AI player
void moveAI(int move[4], int board[8][8], int black_pawns[3][2],
            int white_pawns[3][2]) {

    int min_value = 1000;
    const int direction[8][2] = {{0, 1}, {1, 0},  {0, -1},  {-1, 0},
                                 {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    int found_moves[100][4];
    int moves_number = 0;

    for (int pawn = 0; pawn < 3; pawn++) {
        for (int direction_num = 0; direction_num < 8; direction_num++) {

            int x = black_pawns[pawn][0];
            int y = black_pawns[pawn][1];
            const int previous_x = x;
            const int previous_y = y;
            int dx = direction[direction_num][0];
            int dy = direction[direction_num][1];

            while (checkRange(x + dx) && checkRange(y + dy) &&
                   board[x + dx][y + dy] == 0) {

                x += dx;
                y += dy;

                move[0] = previous_x;
                move[1] = previous_y;
                move[2] = x;
                move[3] = y;

                executeMove(move, black_pawns, board, 3);

                int value = calculateStateValue(board, black_pawns, 3);

                if (value < min_value) {

                    min_value = value;
                    moves_number = 1;
                    for (int i = 0; i < 4; i++)
                        found_moves[0][i] = move[i];

                } else if (value == min_value) {

                    for (int i = 0; i < 4; i++)
                        found_moves[moves_number][i] = move[i];
                    moves_number++;
                }

                move[0] = x;
                move[1] = y;
                move[2] = previous_x;
                move[3] = previous_y;

                executeMove(move, black_pawns, board, 3);
            }
        }
    }

    int move_num = rand() % moves_number;
    for (int i = 0; i < 4; i++) {
        move[i] = found_moves[move_num][i];
    }
}


// check if one of the players won
int checkWinner(const int board[8][8]) {

    if (board[0][7] == 2 && board[1][7] == 2 && board[2][7] == 2) {
        return 2;
    }

    if (board[5][0] == 3 && board[6][0] == 3 && board[7][0] == 3) {
        return 3;
    }

    return 0;
}

int main() {

    srand(time(NULL));

    int board[8][8]{};
    int move[4];
    int black_pawns[3][2] = {{0, 7}, {1, 7}, {2, 7}};
    int white_pawns[3][2] = {{5, 0}, {6, 0}, {7, 0}};
    createBoard(board);

    int winner = 0;

    while (winner == 0) {

        displayBoard(board);

        bool correct_move = 0;

        while (correct_move == 0) {

            displayBoard(board);
            cout << "\033[1;34m";
            cout << "\n   A: ";
            cout << "\033[0m";

            correct_move = readInput(move);
            if (correct_move)
                correct_move = isCorrectMove(move, board);
        }

        executeMove(move, white_pawns, board, 2);

        displayBoard(board);
        cout << "\033[1;31m";
        cout << "\n   B: " << endl;
        cout << "\033[0m";
        sleep(1);

        moveAI(move, board, black_pawns, white_pawns);
        executeMove(move, black_pawns, board, 3);

        winner = checkWinner(board);
    }

    displayBoard(board);

    if (winner == 2)
        cout << "\n  You have won.\n\n";
    else
        cout << "\n  You have lost.\n\n";

    return 0;
}