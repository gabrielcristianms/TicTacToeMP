#include <iostream>

class game_state {
public:

    // The game state is a 1D array with 9 cells.
    // ' ' is empty, 'o' and 'x' are players.
    char board[9] = {
        ' ', ' ', ' ',
        ' ', ' ', ' ',
        ' ', ' ', ' '
    };
    int score_x = 0;
    int score_o = 0;

    // Reads from cell number from terminal.
    // Asserts that chosen cell is valid and empty.
    int read_cell() {
        int cell;
        while (true) {
            std::cin >> cell;
            if (cell < 0 || cell > 9) {
                std::cout << "Cell number must be from 0 to 8, or 9 to end the game." << std::endl;
            } else if (cell == 9) {
                std::cout << "Leaving game..." << std::endl;
                break;
            } else if (board[cell] != ' ') {
                std::cout << "Cell not empty." << std::endl;
            } else {
                break;
            }
        }
        return cell;
    }

    // Gets current winner.
    // 'x' or 'o' to indicate a player.
    // 'd' is a draw.
    // 0 is none.
    char get_winner() {
        for (int i = 0; i < 3; i ++)
            if (board[i*3] == board[1 + i*3] && board[i*3] == board[2 + i*3] && board[i*3] != ' ')
                return board[i*3];
        for (int i = 0; i < 3; i ++)
            if (board[i] == board[i + 3] && board[i] == board[i + 6] && board[i] != ' ')
                return board[i];
        if (board[0] == board[4] && board[4] == board[8] && board[4] != ' ')
            return board[0];
        if (board[2] == board[4] && board[4] == board[6] && board[4] != ' ')
            return board[0];
        for (int i = 0; i < 9; i++)
            if (board[i] == ' ')
                return 0;
        return 'd';
    }

    // Prints to terminal the current game state.
    void print_board() {
        std::cout << " ----- " << std::endl;
        std::cout << "|" << board[0] << ' ' << board[1] << ' ' << board[2] << "|" << std::endl;
        std::cout << "|" << board[3] << ' ' << board[4] << ' ' << board[5] << "|" << std::endl;
        std::cout << "|" << board[6] << ' ' << board[7] << ' ' << board[8] << "|" << std::endl;
        std::cout << " ----- " << std::endl;
    }

    // Should be used on server (player x).
    void play_x(int cell, bool print = true) {
        board[cell] = 'x';
        char winner = get_winner();
        if (winner == 'x') {
            score_x++;
            if (print)
                std::cout << "You won! " << score_x << "x" << score_o << std::endl;
        } else if (winner == 'd') {
            score_x++;
            score_o++;
            if (print)
                std::cout << "Draw. " << score_x << "x" << score_o << std::endl;
        }
    }

    // Should be used on client (player o).
    void play_o(int cell, bool print = true) {
        board[cell] = 'o';
        char winner = get_winner();
        if (winner == 'o') {
            score_o++;
            if (print)
                std::cout << "You won! " << score_x << "x" << score_o << std::endl;
        } else if (winner == 'd') {
            score_x++;
            score_o++;
            if (print)
                std::cout << "Draw. " << score_x << "x" << score_o << std::endl;
        }
    }

    // Resets board.
    void clear() {
        for (int i = 0; i < 9; i++)
            board[i] = ' ';
    }

    // Called after receiving the board state, to check if needs to be cleared.
    void check_end(bool print = true) {
        char winner = get_winner();
        if (winner == 0)
            return;
        if (winner == 'd') {
            if (print)
                std::cout << "Draw. " << score_x << "x" << score_o << std::endl;
            clear();
        } else {
            if (print)
                std::cout << "You lose..." << score_x << "x" << score_o << std::endl;
            clear();
        }
    }

    void clear_on_win(bool print = true) {
        char winner = get_winner();
        if(winner == 'x' or winner == 'o' or winner == 'd'){
            clear();
        }
    }
};        