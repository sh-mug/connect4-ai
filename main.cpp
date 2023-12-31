#include <bits/stdc++.h>
using namespace std;

using turn_t = char;
using cell_t = int;
using board_t = unsigned long long;
using score_t = int;
using score_opt = optional<score_t>;
using reward_t = float;

constexpr int Y = 7, X = 9, L = 4;
constexpr std::array<board_t, 126> lines = { 0x0000000000204081, 0x0000000010204080, 0x0000000810204000, 0x0000040810200000, 0x0002040810000000, 0x0102040800000000, 0x0000000000408102, 0x0000000020408100, 0x0000001020408000, 0x0000081020400000, 0x0004081020000000, 0x0204081000000000, 0x000000000000000f, 0x0000000000000780, 0x000000000003c000, 0x0000000001e00000, 0x00000000f0000000, 0x0000007800000000, 0x00003c0000000000, 0x001e000000000000, 0x0f00000000000000, 0x0000000001010101, 0x0000000080808080, 0x0000004040404000, 0x0000202020200000, 0x0010101010000000, 0x0808080800000000, 0x0000000000208208, 0x0000000010410400, 0x0000000820820000, 0x0000041041000000, 0x0002082080000000, 0x0104104000000000, 0x0000000000810204, 0x0000000040810200, 0x0000002040810000, 0x0000102040800000, 0x0008102040000000, 0x0408102000000000, 0x000000000000001e, 0x0000000000000f00, 0x0000000000078000, 0x0000000003c00000, 0x00000001e0000000, 0x000000f000000000, 0x0000780000000000, 0x003c000000000000, 0x1e00000000000000, 0x0000000002020202, 0x0000000101010100, 0x0000008080808000, 0x0000404040400000, 0x0020202020000000, 0x1010101000000000, 0x0000000000410410, 0x0000000020820800, 0x0000001041040000, 0x0000082082000000, 0x0004104100000000, 0x0208208000000000, 0x0000000001020408, 0x0000000081020400, 0x0000004081020000, 0x0000204081000000, 0x0010204080000000, 0x0810204000000000, 0x000000000000003c, 0x0000000000001e00, 0x00000000000f0000, 0x0000000007800000, 0x00000003c0000000, 0x000001e000000000, 0x0000f00000000000, 0x0078000000000000, 0x3c00000000000000, 0x0000000004040404, 0x0000000202020200, 0x0000010101010000, 0x0000808080800000, 0x0040404040000000, 0x2020202000000000, 0x0000000000820820, 0x0000000041041000, 0x0000002082080000, 0x0000104104000000, 0x0008208200000000, 0x0410410000000000, 0x0000000002040810, 0x0000000102040800, 0x0000008102040000, 0x0000408102000000, 0x0020408100000000, 0x1020408000000000, 0x0000000000000078, 0x0000000000003c00, 0x00000000001e0000, 0x000000000f000000, 0x0000000780000000, 0x000003c000000000, 0x0001e00000000000, 0x00f0000000000000, 0x7800000000000000, 0x0000000008080808, 0x0000000404040400, 0x0000020202020000, 0x0001010101000000, 0x0080808080000000, 0x4040404000000000, 0x0000000001041040, 0x0000000082082000, 0x0000004104100000, 0x0000208208000000, 0x0010410400000000, 0x0820820000000000, 0x0000000004081020, 0x0000000204081000, 0x0000010204080000, 0x0000810204000000, 0x0040810200000000, 0x2040810000000000, 0x0000000008102040, 0x0000000408102000, 0x0000020408100000, 0x0001020408000000, 0x0081020400000000, 0x4081020000000000, };
constexpr board_t MASK_VER = 0b0001111'0001111'0001111'0001111'0001111'0001111'0001111'0001111'0001111;
constexpr board_t MASK_HOR = 0b0000000'0000000'0000000'1111111'1111111'1111111'1111111'1111111'1111111;
constexpr board_t MASK_DG1 = 0b0000000'0000000'0000000'1111000'1111000'1111000'1111000'1111000'1111000;
constexpr board_t MASK_DG2 = 0b0000000'0000000'0000000'0001111'0001111'0001111'0001111'0001111'0001111;
constexpr board_t end_board = 0x7FFFFFFFFFFFFFFF;
constexpr int EXPAND_COUNT = 10;
constexpr reward_t C_UCB = 1.414213562373;
constexpr long TL_PER_ROUND = 0.090 * CLOCKS_PER_SEC;

namespace MMNMM{
    class splitmix64{
        unsigned long x;
    public:
        using result_type = unsigned long;
        constexpr unsigned long max() const { return std::numeric_limits<unsigned long>::max(); }
        constexpr unsigned long min() const { return std::numeric_limits<unsigned long>::min(); }
        constexpr explicit splitmix64(unsigned long seed = 0) : x(seed) {}
        constexpr unsigned long operator()() {
            unsigned long z = (x += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            return z ^ (z >> 31);
        }
    };
}
MMNMM::splitmix64 engine(std::random_device{}());

inline bool is_win(const board_t board) {
    board_t ver = board & (board >> 1);
    ver &= (ver >> 2) & MASK_VER;
    board_t hor = board & (board >> 7);
    hor &= (hor >> 14) & MASK_HOR;
    board_t dg1 = board & (board >> 6);
    dg1 &= (dg1 >> 12) & MASK_DG1;
    board_t dg2 = board & (board >> 8);
    dg2 &= (dg2 >> 16) & MASK_DG2;
    return ver | hor | dg1 | dg2;
}

struct State {
    board_t me, opp;
    inline int count_fill() { return __builtin_popcountll(me | opp); }
    inline score_opt eval() {
        if (is_win(me)) return 2;
        if (is_win(opp)) return -2;
        if ((me | opp) == end_board) return 0;
        return nullopt;
    }
    score_t eval_approx() {
        // return 0;
        array<int, 4> cnt_me = {}, cnt_opp = {};
        for (const board_t &line : lines) {
            ++cnt_me[__builtin_popcountll(me & line)];
            ++cnt_opp[__builtin_popcountll(opp & line)];
        }
        for (int cnt = 3; cnt >= 0; --cnt) {
            if (cnt_me[cnt] > cnt_opp[cnt]) return 1;
            if (cnt_me[cnt] < cnt_opp[cnt]) return -1;
        }
        return 0;
    }
    State reversed() {
        return State{opp, me};
    }
    vector<turn_t> children(bool can_steal) {
        vector<turn_t> res;
        board_t filled = me | opp;
        for (int x = 0; x < X; ++x) {
            turn_t cell = Y * x;
            for (int y = 0; y < Y; ++y, ++cell) if (!(filled >> cell & 1)) {
                res.push_back(cell);
                break;
            }
        }
        if (can_steal) res.push_back(-2);
        return res;
    }
    State played(turn_t cell) {
        if (cell >= 0) return State{opp, me | (1ULL << cell)};
        return State{me, opp};
    }

    State() : me(0), opp(0) {}
    State(board_t me, board_t opp) : me(me), opp(opp) {}
    bool operator<(const State &rhs) const { return make_pair(me, opp) < make_pair(rhs.me, rhs.opp); }
};
std::ostream& operator<<(std::ostream& os, const State& st) {
    for (int y = Y - 1; y >= 0; --y) {
        for (int x = 0; x < X; ++x) {
            board_t cell = 1ULL << (Y * x + y);
            os << (st.me & cell ? 'O' : st.opp & cell ? 'X' : '.')
               << (x + 1 == X ? '\n' : ' ');
        }
    }
    return os;
}

inline int to_row(cell_t cell) {
    if (cell >= 0) return cell / Y;
    return cell;
}
struct Result {
    score_t score; cell_t cell;
    int row() { return to_row(cell); }

    Result() : score(0), cell(-1) {}
    Result(score_t score, cell_t cell) : score(score), cell(cell) {}
    Result operator-() const { return Result{-score, cell}; }
    bool operator<(const Result &rhs) const { return score < rhs.score; }
    bool operator>=(const Result &rhs) const { return score >= rhs.score; }
};
std::map<std::pair<State, int>, Result> memo;
Result alpha_beta(State state, int depth, score_t alpha, score_t beta) {
    auto memo_input = std::make_pair(state, depth);
    if (memo.find(memo_input) != memo.end()) return memo[memo_input];
    
    score_opt h = state.eval();
    if (h) {
        return memo[memo_input] = Result{h.value(), -1};
    }
    if (depth == 0) {
        const auto children = state.children(false);
        return memo[memo_input] = Result{state.eval_approx(), children[children.size() >> 1]};
    }

    Result res = Result{alpha, -1};
    for (const turn_t next_turn : state.children(false)) {
        State next = state.played(next_turn);
        Result next_res = -alpha_beta(next, depth - 1, -beta, -alpha);
        if (res < next_res) res = Result{next_res.score, next_turn};
        if (res.score >= beta) return memo[memo_input] = res;
    }
    return memo[memo_input] = res;
}

struct MCTSnode {
    State state;
    reward_t reward;
    int cnt;
    char turn, depth;
    unique_ptr<vector<MCTSnode>> children;

    reward_t add_reward(reward_t added_reward) {
        assert(-2 <= added_reward && added_reward <= 2);
        reward += (added_reward + 1) / 2;
        ++cnt;
        return added_reward;
    }
    inline reward_t eval() {
        score_opt h = state.eval();
        if (h) return add_reward(h.value());

        if (!children) {
            reward_t rollout_reward = rollout();
            if (cnt >= EXPAND_COUNT) expand();
            return add_reward(rollout_reward);
        } else {
            MCTSnode *largest = largest_ucb_child();
            reward_t child_reward = largest->eval();
            return add_reward(-child_reward);
        }
    }
    inline reward_t rollout() {
        bool is_me = true;
        State now_state = state;
        score_opt h;
        while (!(h = now_state.eval())) {
            auto next_turns = now_state.children(depth == 1);
            auto next_turn = next_turns[engine() % next_turns.size()];
            now_state = now_state.played(next_turn);
            is_me = !is_me;
        }
        return (is_me ? h.value() : -h.value());
    }
    inline void expand() {
        children = make_unique<vector<MCTSnode>>();
        for (cell_t next_turn : state.children(depth == 1)) {
            children->push_back(MCTSnode{state.played(next_turn), next_turn, depth + 1});
        }
    }
    inline MCTSnode *largest_ucb_child() {
        reward_t largest = -1e9;
        MCTSnode *largest_child = nullptr;
        for (MCTSnode &child : *children) {
            if (child.cnt == 0) return &child;
            reward_t ucb = (-child.reward / child.cnt) + C_UCB * sqrt(log((reward_t)cnt) / child.cnt);
            if (largest < ucb) {
                largest = ucb;
                largest_child = &child;
            }
        }
        return largest_child;
    }
    MCTSnode *max_cnt_child() {
        int largest = -1e9;
        MCTSnode *largest_child = nullptr;
        for (MCTSnode &child : *children) {
            if (child.cnt == 0) return &child;
            if (largest < child.cnt) {
                largest = child.cnt;
                largest_child = &child;
            }
        }
        return largest_child;
    }
    void learn(long TL) {
        clock_t deadline = TL + clock();
        int num_sims = 0;
        while ((++num_sims & 0x1F) || clock() < deadline) {
            eval();
        }
        cerr << "#sim=" << num_sims << " reward=" << reward << endl;
    }
    MCTSnode *choose_node() {
        MCTSnode *largest_child = max_cnt_child();
        for (const auto &child : *children) {
            cerr << (int)child.turn << ' ' << child.cnt << ' ' << child.reward << endl;
        }
        return largest_child;
    }
    
    MCTSnode(State state, turn_t turn, char depth) : state(state), reward(0), cnt(0), children(nullptr), turn(turn), depth(depth) {}
    MCTSnode() : state(State{}), reward(0), cnt(0), children(nullptr), turn(-1), depth(0) {}
};
std::ostream& operator<<(std::ostream& os, const MCTSnode& node) {
    cerr << "reward=" << node.reward;
    cerr << " count=" << node.cnt;
    cerr << " turn=" << (int)node.turn << endl;
    cerr << "children=";
    if (!node.children) {
        cerr << "null" << endl;
    } else {
        for (auto &child : *node.children) cerr << (int)child.turn << ',';
        cerr << endl;
    }
    cerr << node.state;
    return os;
}

atomic<bool> is_opp_entered(false);
void bg_operation(MCTSnode *node) {
    int num_sims = 0;
    while (!is_opp_entered) {
        ++num_sims;
        node->eval();
    }
    cerr << "#sim(background)=" << num_sims << endl;
}

int main(void)
{
    MCTSnode *node = new MCTSnode;

    int my_id; // 0 or 1 (Player 0 plays first)
    int opp_id; // if your index is 0, this will be 1, and vice versa
    cin >> my_id >> opp_id; cin.ignore();

    // game loop
    while (1) {
        is_opp_entered = false;
        thread bg_thread(bg_operation, node);

        int turn_index; // starts from 0; As the game progresses, first player gets [0,2,4,...] and second player gets [1,3,5,...]
        cin >> turn_index; cin.ignore();
        for (int i = 0; i < 7; i++) {
            string board_row; // one row of the board (from top to bottom)
            cin >> board_row; cin.ignore();
        }
        int num_valid_actions; // number of unfilled columns in the board
        cin >> num_valid_actions; cin.ignore();
        for (int i = 0; i < num_valid_actions; i++) {
            int action; // a valid column index into which a chip can be dropped
            cin >> action; cin.ignore();
        }
        int opp_previous_action; // opponent's previous chosen column index (will be -1 for first player in the first turn)
        cin >> opp_previous_action;

        is_opp_entered = true;
        bg_thread.join();

        if (opp_previous_action != -1) {
            if (!node->children) node->expand();
            for (MCTSnode &child : *node->children) {
                if (to_row(child.turn) == opp_previous_action) {
                    node = &child;
                    break;
                }
            }
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        node->learn(node->depth <= 1 ? 0.950 * CLOCKS_PER_SEC : TL_PER_ROUND);
        node = node->choose_node();
        cerr << *node;
        cout << to_row(node->turn) << endl;

        // Output a column index to drop the chip in. Append message to show in the viewer.
        // cout << to_row(node.turn) << endl;
    }
}