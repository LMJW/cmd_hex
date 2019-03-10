/// Created by Liming J
#include <climits>
#include <exception>
#include <functional>
#include <iostream>
#include <ostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
enum class node_status : short { BLANK, BLUE, RED };

/// Define a abstract base player class to play the hex game
/// Use as a interface
class Player {
public:
    Player() {}
    ~Player() {}
    virtual pair<int, int> play() = 0;
};

struct HexNode {
    /// Node does not know what position it is,
    /// it only know and care about its status.
    /// thus, the position info is not stored in the node object
    node_status current;
    HexNode() : current(node_status::BLANK) {}
    HexNode(const HexNode& n) : current(n.current) {}
    ~HexNode() {}
    void move(unsigned int move) {
        switch (move) {
            case 1:
                current = node_status::BLUE;
                break;
            case 2:
                current = node_status::RED;
                break;
            default:
                break;
        }
    }
    friend ostream& operator<<(ostream& o, HexNode& hn) {
        switch (hn.current) {
            case node_status::BLUE:
                o << "X";
                break;
            case node_status::RED:
                o << "O";
                break;
            default:
                o << ".";
                break;
        }
        return o;
    }
};

class HexBoard {
public:
    /// Define friend class so the Player can access the private data
    friend class Player;

    HexBoard() {}

    /// @pram n use 1-d array to store all the nodes
    /// nodes can be accessed through r*grid_n+c
    HexBoard(unsigned int n)
        : nodes(n * n, HexNode()), grid_n(n), turns(true) {}

    HexBoard(unsigned int n, Player& p1, Player& p2)
        : nodes(n * n, HexNode()),
          grid_n(n),
          turns(true),
          redP(&p1),
          blueP(&p2) {}

    /// copy constructor
    HexBoard(const HexBoard& x)
        : nodes(x.nodes), grid_n(x.grid_n), turns(x.turns), filled(x.filled) {}
    ~HexBoard() {}

    void registerRedPlayer(Player& p) { redP = &p; }

    void registerBluePlayer(Player& p) { blueP = &p; }

    /// if node is still blank, then it's a valid move
    /// @pram idx = r*grid_n+c
    bool check_blank(unsigned int idx) {
        return nodes[idx].current == node_status::BLANK;
    }

    bool check_is_blue(unsigned int idx) {
        return nodes[idx].current == node_status::BLUE;
    }

    bool check_is_red(unsigned int idx) {
        return nodes[idx].current == node_status::RED;
    }

    /// Check the hexnode color to see which player took the spot
    int check_hexnode_player(unsigned int idx) {
        if (nodes[idx].current == node_status::BLUE) {
            return 1;
        } else if (nodes[idx].current == node_status::RED) {
            return 2;
        }
        return 0;
    }

    /// To determine if one player win, I will use a DFS search to find a path
    /// from one edge to opposite edge. If I can find, then that means this
    /// player did not win.
    ///
    /// Assume the first player, BLUE, needs to find a path from top to bottom.
    /// And the second player, RED, needs to go from left to right.
    ///
    /// To check if player blue win, we need to start from left side, do a dfs
    /// search to see if we can get to the right side of grid. If we cannot,
    /// that means the player blue wins. Conversely, we need to start from top
    /// edge to do dfs and see if we can get to the bottom edge.

    /// @pram using int to indicate which player we are talking about
    /// @return true if this player wins
    bool check_win(int player) {
        vector<unsigned int> start_nodes;
        unordered_set<unsigned int> end_nodes;
        switch (player) {
            case 1:  /// if player blue wins, that means no path from left to
                     /// right edge
                for (unsigned int i = 0; i < grid_n; ++i) {
                    if (!check_is_blue(index_of(i, 0))) {
                        /// the start point can only be no blue hex
                        start_nodes.push_back(index_of(i, 0));
                    }
                    if (!check_is_blue(index_of(i, grid_n - 1))) {
                        /// the end point must not be blue point
                        end_nodes.insert(index_of(i, grid_n - 1));
                    }
                }
                break;

            case 2:  /// if player red wins, that means no path from top to
                     /// bottom
                for (unsigned int i = 0; i < grid_n; ++i) {
                    if (!check_is_red(index_of(0, i))) {
                        start_nodes.push_back(index_of(0, i));
                    }
                    if (!check_is_red(index_of(grid_n - 1, i))) {
                        end_nodes.insert(index_of(grid_n - 1, i));
                    }
                }
                break;
        }
        unordered_set<unsigned int> has_gone;
        return _dfs_(start_nodes, end_nodes, has_gone, player);
    }

    void draw() {
        /// The draw include draw the node and the connection between nodes
        /// the number of connections between nodes has 1 less than number of
        /// nodes
        ///
        /// eg: if we have 7 nodes, we will have 6 lines connect all these nodes
        /// we can do this by having 7*2-1 = 13, and draw line when we have odd
        /// indexes
        bool even_row = true;
        auto l = grid_n * 2 - 1;
        for (int i = 0; i < l; ++i) {
            if (even_row) {
                for (int j = 0; j < i; ++j) {
                    cout << " ";  /// white spaces used for aligning the rows
                }
                /// print the row with nodes
                bool even = true;
                for (int j = 0; j < l; ++j) {
                    if (even) {
                        cout << nodes[index_of(i / 2, j / 2)];
                    } else {
                        cout << " - ";
                    }
                    even = !even;
                }
            } else {
                /// print the seperation row
                /// print the blank space to align the graph
                for (int j = 0; j < i; ++j) {
                    cout << " ";
                }
                bool even = true;
                for (int j = 0; j < l; ++j) {
                    if (even) {
                        cout << "\\ ";
                    } else {
                        cout << "/ ";
                    }
                    even = !even;
                }
            }
            even_row = !even_row;
            cout << endl;
        }
    }

    /// @pram r, c the index of grid where player want to place their hex
    /// @return boolean to confirm whether player's move is successful
    /// if the move is not a valid move, a false will returned and the reason
    /// will be printed on the stdout
    bool place_hex(unsigned int r, unsigned int c) {
        unsigned int position;
        try {
            position = index_of(r, c);
        } catch (invalid_argument e) {
            cout << e.what() << endl;
            cout << "please re-enter a correct move:\n";
            return false;
        }
        if (check_blank(position)) {
            if (turns) {
                nodes[position].move(1);
            } else {
                nodes[position].move(2);
            }
            ++filled;
            return true;
        }
        return false;
    }

    void start_game() {
        cout << "Game started. Press Ctrl+C if you want to quit game\n";
        cout << "the grid is a " << grid_n << "x" << grid_n << " grid.\n";
        cout << "The index starts from up left corner with index 0,0, and the "
                "down right corner with index "
             << grid_n - 1 << "," << grid_n - 1 << ".\n";
        cout << "You need to enter the index r c seperate with space to place "
                "your hex on the grid\n";
        draw();
        auto bluewin = check_win(1);
        auto redwin = check_win(2);
        unsigned int r, c;
        while (!bluewin && !redwin) {
            if (turns) {
                cout << "Now is Blue player's turn:\n";
                current_p = blueP;
            } else {
                cout << "Now is Red player's turn:\n";
                current_p = redP;
            }
            /// check if the input is valid, if it is not valid, player need
            /// to re-enter the next move
            auto p = current_p->play();
            r = p.first;
            c = p.second;
            while (!place_hex(r, c)) {
                cout << "(" << r << "," << c
                     << ") is invalid, please re-enter:\n";
                auto p = current_p->play();
                r = p.first;
                c = p.second;
            }

            turns = !turns;
            draw();
            bluewin = check_win(1);
            redwin = check_win(2);
        }
        cout << "Game over!!!\nThe winner is ";
        if (bluewin) {
            cout << "BLUE\n!";
        } else if (redwin) {
            cout << "RED\n!";
        }
    }
    /// base data of Hexboard
    vector<HexNode> nodes;
    unsigned int grid_n;
    bool turns;  /// Indicate the current turn player : true->Blue ; false->Red
    int filled;

private:
    Player* redP;
    Player* blueP;
    Player* current_p;
    unsigned int index_of(unsigned int r, unsigned int c) const {
        if (r > grid_n || c > grid_n) {
            throw invalid_argument("input out of bound.");
        };
        return r * grid_n + c;
    }

    /// given a node index, find all its neighbors
    vector<unsigned int> get_neighbors(unsigned int idx) const {
        /// 1-d index to actual r, c index
        unsigned int r = idx / grid_n;
        unsigned int c = idx % grid_n;

        vector<unsigned int> neis;
        /// six possible neighbor options
        if (c + 1 < grid_n) {
            if (r > 0) {
                neis.push_back(index_of(r - 1, c + 1));
            };
            neis.push_back(index_of(r, c + 1));
        };
        if (r + 1 < grid_n) {
            neis.push_back(index_of(r + 1, c));
        };
        if (r > 0) {
            neis.push_back(index_of(r - 1, c));
        };
        if (c > 0) {
            if (r + 1 < grid_n) {
                neis.push_back(index_of(r + 1, c - 1));
            };
            neis.push_back(index_of(r, c - 1));
        };

        return neis;
    }

    /// @pram start , a vector stores all the possible start points
    /// @pram end set, used to check if a node gets to the opposite edge
    /// @pram player, use to indicate the player. The reason for having this
    /// player is because in the DFS process, if a hex is occupied by other
    /// player, then the current search needs to eliminate this hex in search
    /// process. Player red and blue will not be able to reuse opponents' cell,
    /// that's why we need player index here;
    ///
    bool _dfs_(vector<unsigned int>& start,
               const unordered_set<unsigned int>& end,
               unordered_set<unsigned int>& has_gone,
               const unsigned int& player) {
        for (auto sn : start) {
            has_gone.insert(sn);
            auto iter = end.find(sn);
            if (iter != end.end()) {
                /// can find a path, thus player does not win
                return false;
            }
            const auto& nbs = get_neighbors(sn);
            vector<unsigned int> nextstart;

            if (player == 1) {  /// check player blue win or not
                /// so we check using all none blue hex to see if if connect
                /// from left to right
                for (auto e : nbs) {
                    auto it = has_gone.find(e);
                    if (it == has_gone.end() && !check_is_blue(e)) {
                        nextstart.push_back(e);
                    };
                }
            } else {
                /// check player red win or not
                /// so we need to use none-red hex
                for (auto e : nbs) {
                    auto it = has_gone.find(e);

                    if (it == has_gone.end() && !check_is_red(e)) {
                        nextstart.push_back(e);
                    };
                }
            }
            /// if we found a path, that means the current play does not win
            /// so we will directly return. Otherwise, we need to continue
            /// search untill all possible path are searched.
            if (!_dfs_(nextstart, end, has_gone, player)) {
                return false;
            };
        }
        return true;
    }
};

/// Define human player to take input from stdin
class Human : public Player {
public:
    Human() : Player() {}
    pair<int, int> play() {
        int r, c;
        cin >> r >> c;
        return make_pair(r, c);
    }
};

/// Define a MonteCarlo player to enter input after simulation. To initialize a
/// montecarlo player, we need to pass two parameter, one is the number of
/// simulation, and the other is the hexboard state. The hexboard state is
/// copied use a copy constructor.

class MonteCarlo : public Player {
public:
    /// snapshot the state of current hexboard
    MonteCarlo(int n, HexBoard& h) : Player(), simu(n), hb(&h) {
        getsnapshot();
        srand(9);
    }
    /// Simulate the play using monte carlo
    int simulate() {
        /// Simulate the play
        for (int i = 0; i < simu; ++i) {
            /// play until the whole hexboard is filled
            /// we can add a counter inside the hexboard to keep track on how
            /// many moves have been made. We will randomly pick a number to
            /// place hex. If it cannot fill, we will make another random move
            /// until we fill all hexes.
            auto ns = HexBoard(snapshot);
            while (ns.filled < ns.nodes.size()) {
                auto next_move = rand() % ns.nodes.size();
                int r = next_move / ns.grid_n;
                int c = next_move % ns.grid_n;
                if (ns.check_blank(next_move)) {
                    ns.place_hex(r, c);
                }
                ns.turns = !ns.turns;
            }
            /// Check if player blue wins. if not, then player red wins as hex
            /// cannot be draw and will always have a winner.
            /// check snapshot turns to determine which player should move and
            /// that player's winning condition
            bool win;
            int player;
            if (snapshot.turns) {
                player = 1;  /// blue player
            } else {
                player = 2;  /// red player
            }
            win = ns.check_win(player);
            if (win) {
                for (int i = 0; i < ns.nodes.size(); ++i) {
                    if (!snapshot.check_blank(i)) {
                        continue;
                    }
                    if (ns.check_hexnode_player(i) == player) {
                        if (wincount.find(i) == wincount.end()) {
                            wincount[i] = 1;
                        } else {
                            ++wincount[i];
                        }
                    }
                }
            } else {
                for (int i = 0; i < ns.nodes.size(); ++i) {
                    if (!snapshot.check_blank(i)) {
                        continue;
                    }
                    if (ns.check_hexnode_player(i) == player) {
                        if (wincount.find(i) == wincount.end()) {
                            wincount[i] = -1;
                        } else {
                            --wincount[i];
                        }
                    }
                }
            }
        }
        /// Find the position that leads to most wins
        int idx = 0, v = INT_MIN;
        for (auto it = wincount.begin(); it != wincount.end(); ++it) {
            if (it->second > v) {
                idx = it->first;
                v = it->second;
            }
        }

        return idx;
    }

    /// For every move, get a snapshot of current hexboard state and
    /// re-initialize the unordered map
    void getsnapshot() {
        snapshot = HexBoard(*hb);
        wincount = unordered_map<int, int>{};
    }

    pair<int, int> play() {
        getsnapshot();
        int g_n = snapshot.grid_n;
        int idx = simulate();

        return make_pair(idx / g_n, idx % g_n);
    }

private:
    int simu;
    unordered_map<int, int> wincount;
    HexBoard* hb;
    HexBoard snapshot;
};

int main() {
    HexBoard hb(11);  /// Specify the hexboard size, you can change this number
                      /// to a smaller number if you want. :)
    Human p1;  /// You are this player, you can enter your move through stdin.
    /// MonteCarlo p1(1000, hb);
    MonteCarlo p2(1000, hb);
    hb.registerBluePlayer(p1);
    hb.registerRedPlayer(p2);
    hb.start_game();
}