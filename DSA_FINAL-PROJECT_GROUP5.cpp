#include <iostream>
#include <queue>
#include <string>

using namespace std;

// Binary Tree for Player Names
struct PlayerNode {
    string name;
    PlayerNode* left;
    PlayerNode* right;
    PlayerNode(string playerName) : name(playerName), left(nullptr), right(nullptr) {}
};

// Binary Search Tree (BST) for storing moves
struct MoveNode {
    string move;
    MoveNode* left;
    MoveNode* right;
    MoveNode(string m) : move(m), left(nullptr), right(nullptr) {}
};

// Max-Heap for storing recent moves
struct CompareMove {
    bool operator()(string move1, string move2) {
        return move1 < move2;  // Max-Heap
    }
};

class CheckersGame {
private:
    PlayerNode* redPlayerTree;
    PlayerNode* blackPlayerTree;
    MoveNode* moveHistoryRoot;
    priority_queue<string, vector<string>, CompareMove> recentMoves;
    char board[8][8];
    int redScore;  // Track Red player's score
    int blackScore;  // Track Black player's score

    // Function to initialize the game board
    void initializeBoard() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if ((i + j) % 2 == 0) {
                    board[i][j] = ' ';  // Empty space
                } else if (i < 3) {
                    board[i][j] = 'R';  // Red pieces
                } else if (i > 4) {
                    board[i][j] = 'B';  // Black pieces
                } else {
                    board[i][j] = ' ';  // Empty space in the middle rows
                }
            }
        }
    }

    // Function to print the current state of the board with labels
    void printBoard() {
        cout << "   0 1 2 3 4 5 6 7" << endl;  // Print column numbers
        for (int i = 0; i < 8; ++i) {
            cout << i << " ";  // Print row number
            for (int j = 0; j < 8; ++j) {
                cout << board[i][j] << " ";  // Print the board content
            }
            cout << endl;
        }
    }

    // Insert player name into Binary Tree
    void insertPlayer(PlayerNode*& root, const string& playerName) {
        if (!root) {
            root = new PlayerNode(playerName);
        } else if (playerName < root->name) {
            insertPlayer(root->left, playerName);
        } else {
            insertPlayer(root->right, playerName);
        }
    }

    // Insert move into BST
    void insertMove(MoveNode*& root, const string& move) {
        if (!root) {
            root = new MoveNode(move);
        } else if (move < root->move) {
            insertMove(root->left, move);
        } else {
            insertMove(root->right, move);
        }
    }

    // Function to check if a move is a valid jump (capture)
    bool isValidJump(int x1, int y1, int x2, int y2, char player) {
        int midX = (x1 + x2) / 2;  // Middle square's x-coordinate
        int midY = (y1 + y2) / 2;  // Middle square's y-coordinate

        if (board[x2][y2] == ' ' && board[midX][midY] != ' ' &&
            ((player == 'R' && board[midX][midY] == 'B') || (player == 'B' && board[midX][midY] == 'R'))) {
            return true;
        }
        return false;
    }

    // Function to check if a piece should be kinged
    void kingPiece(int x, int y, char player) {
        if ((player == 'R' && x == 0) || (player == 'B' && x == 7)) {
            board[x][y] = toupper(board[x][y]);  // King the piece (turn it to 'K')
        }
    }

    // Function to update the score
    void updateScore(char player) {
        if (player == 'R') {
            redScore++;  // Increase Red's score when capturing
        } else if (player == 'B') {
            blackScore++;  // Increase Black's score when capturing
        }
    }

public:
    CheckersGame() : redPlayerTree(nullptr), blackPlayerTree(nullptr), moveHistoryRoot(nullptr), redScore(0), blackScore(0) {
        initializeBoard();
    }

    // Function to input player names
    void inputPlayerNames() {
        string redName, blackName;
        cout << "Enter Red Player's name: ";
        cin >> redName;
        insertPlayer(redPlayerTree, redName);

        cout << "Enter Black Player's name: ";
        cin >> blackName;
        insertPlayer(blackPlayerTree, blackName);
    }

    // Function to make a move based on the input coordinates
    bool makeMove(char player, int x1, int y1, int x2, int y2) {
        if (x1 < 0 || x1 >= 8 || y1 < 0 || y1 >= 8 || x2 < 0 || x2 >= 8 || y2 < 0 || y2 >= 8) {
            return false;  // Out of bounds
        }

        // Check if the piece belongs to the correct player
        if ((player == 'R' && board[x1][y1] != 'R' && board[x1][y1] != 'K') ||
            (player == 'B' && board[x1][y1] != 'B' && board[x1][y1] != 'K')) {
            return false;  // Incorrect piece
        }

        // Jump (capture opponent's piece)
        if (isValidJump(x1, y1, x2, y2, player)) {
            int midX = (x1 + x2) / 2;
            int midY = (y1 + y2) / 2;
            board[x2][y2] = board[x1][y1];  // Move the piece
            board[x1][y1] = ' ';  // Remove from original position
            board[midX][midY] = ' ';  // Remove opponent's piece
            updateScore(player);  // Update the score
        }
        // Regular move (forward or backward)
        else if (board[x2][y2] == ' ') {
            board[x2][y2] = board[x1][y1];
            board[x1][y1] = ' ';
        } else {
            return false;  // Invalid move
        }

        // Check for kinging
        kingPiece(x2, y2, player);

        // Insert the move into BST and max-heap
        string move = string(1, player) + "(" + to_string(x1) + "," + to_string(y1) + ")->(" + to_string(x2) + "," + to_string(y2) + ")";
        insertMove(moveHistoryRoot, move);
        recentMoves.push(move);

        return true;
    }

    // Display the most recent move from Heap
    void displayRecentMove() {
        if (!recentMoves.empty()) {
            cout << "Most recent move: " << recentMoves.top() << endl;
        } else {
            cout << "No moves made yet." << endl;
        }
    }

    // Display the current scores
    void displayScores() {
        cout << "Red Player's Score: " << redScore << endl;
        cout << "Black Player's Score: " << blackScore << endl;
    }

    // Start the game
    void startGame() {
        while (true) {
            printBoard();
            displayScores();  // Display the current scores after each move
            int x1, y1, x2, y2;
            char player;

            cout << "Enter the player (R for Red, B for Black, Q to quit): ";
            cin >> player;

            if (player == 'Q' || player == 'q') {
                cout << "Thanks for playing! Hope you had fun! Goodbye." << endl;
                break;  // Quit the game
            }

            cout << "Enter move coordinates (x1 y1 x2 y2): ";
            cin >> x1 >> y1 >> x2 >> y2;

            if (makeMove(player, x1, y1, x2, y2)) {
                cout << "Move made successfully!" << endl;
                displayRecentMove();
            } else {
                cout << "Invalid move. Try again." << endl;
            }
        }
    }
};

int main() {

    cout<<"****FINAL PROJECT (GROUP 5)****"<<endl;
    cout<<"1. Alessandra Pauleen C. Ranera"<<endl;
    cout<<"2. Argie L. Olaguer"<<endl;
    cout<<"3. Xander James M. Mata\n"<<endl;

    CheckersGame game;
    game.inputPlayerNames();
    game.startGame();
    return 0;
}