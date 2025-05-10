#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include<string>

using namespace std;

// 3x3 grid as a 2D vector
using Board = vector<vector<int>>;

// Structure for puzzle state
struct State
{
    Board board;
    int moves, heuristic, total;
    int blank_r, blank_c;
    State *parent;
    string move;
    int moved_tile;

    // Constructor
    State(Board b, int m = 0, int h = 0) : board(b), moves(m), heuristic(h), total(m + h),
                                           parent(nullptr), moved_tile(-1)
    {
        // Find blank position
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (board[i][j] == 0)
                {
                    blank_r = i;
                    blank_c = j;
                    return;
                }
            }
        }
    }

    // For priority queue
    bool operator>(const State &other) const
    {
        return total > other.total;
    }
};

// For comparing boards
struct BoardCompare
{
    bool operator()(const Board &a, const Board &b) const
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (a[i][j] != b[i][j])
                    return a[i][j] < b[i][j];
            }
        }
        return false;
    }
};

// Calculate Manhattan distance
int manhattan(const Board &current, const Board &goal)
{
    int distance = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int tile = current[i][j];
            if (tile == 0)
                continue;

            // Find tile position in goal
            for (int gi = 0; gi < 3; gi++)
            {
                for (int gj = 0; gj < 3; gj++)
                {
                    if (goal[gi][gj] == tile)
                    {
                        distance += abs(i - gi) + abs(j - gj);
                        break;
                    }
                }
            }
        }
    }
    return distance;
}

// Check if puzzle is solvable
bool is_solvable(const Board &board)
{
    vector<int> tiles;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 0)
                tiles.push_back(board[i][j]);

    int inv = 0;
    for (size_t i = 0; i < tiles.size(); i++)
        for (size_t j = i + 1; j < tiles.size(); j++)
            if (tiles[i] > tiles[j])
                inv++;

    return inv % 2 == 0;
}

// Display the board
void print_board(const Board &board)
{
    cout << "+---+---+---+\n";
    for (const auto &row : board)
    {
        cout << "| ";
        for (int tile : row)
            cout << (tile ? to_string(tile) : " ") << " | ";
        cout << "\n+---+---+---+\n";
    }
}

// Solve the puzzle using A* search
vector<State *> solve(const Board &start, const Board &goal)
{
    // Possible moves
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    const string dirs[] = {"UP", "DOWN", "LEFT", "RIGHT"};

    // Priority queue for states
    priority_queue<State *, vector<State *>, greater<State *>> frontier;
    set<Board, BoardCompare> visited;

    // Create initial state
    State *initial = new State(start, 0, manhattan(start, goal));
    frontier.push(initial);

    while (!frontier.empty())
    {
        State *current = frontier.top();
        frontier.pop();

        // Check if reached goal
        if (current->board == goal)
        {
            // Reconstruct path
            vector<State *> path;
            State *state = current;
            while (state)
            {
                path.push_back(state);
                state = state->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // Mark as visited
        visited.insert(current->board);

        // Try all four moves
        for (int i = 0; i < 4; i++)
        {
            int nr = current->blank_r + dr[i];
            int nc = current->blank_c + dc[i];

            if (nr >= 0 && nr < 3 && nc >= 0 && nc < 3)
            {
                // Create new board
                Board new_board = current->board;
                int tile = new_board[nr][nc];
                swap(new_board[current->blank_r][current->blank_c], new_board[nr][nc]);

                // Skip if visited
                if (visited.count(new_board))
                    continue;

                // Create new state
                State *next = new State(
                    new_board,
                    current->moves + 1,
                    manhattan(new_board, goal));
                next->parent = current;
                next->move = dirs[i];
                next->moved_tile = tile;

                frontier.push(next);
            }
        }
    }

    return {}; // No solution
}

int main()
{
    cout << "8-Puzzle Solver\n\n";

    // Get initial board
    cout << "Enter initial state (use 0 for blank):\n";
    Board initial(3, vector<int>(3));
    for (int i = 0; i < 3; i++)
    {
        cout << "Row " << i + 1 << ": ";
        for (int j = 0; j < 3; j++)
            cin >> initial[i][j];
    }

    // Get goal board
    cout << "\nEnter goal state:\n";
    Board goal(3, vector<int>(3));
    for (int i = 0; i < 3; i++)
    {
        cout << "Row " << i + 1 << ": ";
        for (int j = 0; j < 3; j++)
            cin >> goal[i][j];
    }

    // Show boards
    cout << "\nInitial state:\n";
    print_board(initial);
    cout << "\nGoal state:\n";
    print_board(goal);

    // Check solvability
    if (is_solvable(initial) != is_solvable(goal))
    {
        cout << "\nThis puzzle is not solvable!\n";
        return 0;
    }

    // Solve puzzle
    cout << "\nSolving...\n";
    auto solution = solve(initial, goal);

    // Show solution
    if (solution.empty())
    {
        cout << "No solution found.\n";
    }
    else
    {
        cout << "Solution found in " << solution.size() - 1 << " moves!\n\n";

        for (size_t i = 0; i < solution.size(); i++)
        {
            if (i > 0)
            {
                cout << "Step " << i << ": Move tile "
                     << solution[i]->moved_tile << " " << solution[i]->move << "\n";
            }
            else
            {
                cout << "Initial state:\n";
            }
            print_board(solution[i]->board);
            cout << "\n";
        }

        // Free memory
        for (auto state : solution)
            delete state;
    }

    return 0;
}