#include <bits/stdc++.h>
using namespace std;

using cell_t = int;
using board_t = unsigned long long;
using score_t = int;
using score_opt = optional<score_t>;
using reward_t = float;

constexpr int Y = 7, X = 9, L = 4;
constexpr std::array<board_t, 126> lines = { 0x0000000000204081, 0x0000000010204080, 0x0000000810204000, 0x0000040810200000, 0x0002040810000000, 0x0102040800000000, 0x0000000000408102, 0x0000000020408100, 0x0000001020408000, 0x0000081020400000, 0x0004081020000000, 0x0204081000000000, 0x000000000000000f, 0x0000000000000780, 0x000000000003c000, 0x0000000001e00000, 0x00000000f0000000, 0x0000007800000000, 0x00003c0000000000, 0x001e000000000000, 0x0f00000000000000, 0x0000000001010101, 0x0000000080808080, 0x0000004040404000, 0x0000202020200000, 0x0010101010000000, 0x0808080800000000, 0x0000000000208208, 0x0000000010410400, 0x0000000820820000, 0x0000041041000000, 0x0002082080000000, 0x0104104000000000, 0x0000000000810204, 0x0000000040810200, 0x0000002040810000, 0x0000102040800000, 0x0008102040000000, 0x0408102000000000, 0x000000000000001e, 0x0000000000000f00, 0x0000000000078000, 0x0000000003c00000, 0x00000001e0000000, 0x000000f000000000, 0x0000780000000000, 0x003c000000000000, 0x1e00000000000000, 0x0000000002020202, 0x0000000101010100, 0x0000008080808000, 0x0000404040400000, 0x0020202020000000, 0x1010101000000000, 0x0000000000410410, 0x0000000020820800, 0x0000001041040000, 0x0000082082000000, 0x0004104100000000, 0x0208208000000000, 0x0000000001020408, 0x0000000081020400, 0x0000004081020000, 0x0000204081000000, 0x0010204080000000, 0x0810204000000000, 0x000000000000003c, 0x0000000000001e00, 0x00000000000f0000, 0x0000000007800000, 0x00000003c0000000, 0x000001e000000000, 0x0000f00000000000, 0x0078000000000000, 0x3c00000000000000, 0x0000000004040404, 0x0000000202020200, 0x0000010101010000, 0x0000808080800000, 0x0040404040000000, 0x2020202000000000, 0x0000000000820820, 0x0000000041041000, 0x0000002082080000, 0x0000104104000000, 0x0008208200000000, 0x0410410000000000, 0x0000000002040810, 0x0000000102040800, 0x0000008102040000, 0x0000408102000000, 0x0020408100000000, 0x1020408000000000, 0x0000000000000078, 0x0000000000003c00, 0x00000000001e0000, 0x000000000f000000, 0x0000000780000000, 0x000003c000000000, 0x0001e00000000000, 0x00f0000000000000, 0x7800000000000000, 0x0000000008080808, 0x0000000404040400, 0x0000020202020000, 0x0001010101000000, 0x0080808080000000, 0x4040404000000000, 0x0000000001041040, 0x0000000082082000, 0x0000004104100000, 0x0000208208000000, 0x0010410400000000, 0x0820820000000000, 0x0000000004081020, 0x0000000204081000, 0x0000010204080000, 0x0000810204000000, 0x0040810200000000, 0x2040810000000000, 0x0000000008102040, 0x0000000408102000, 0x0000020408100000, 0x0001020408000000, 0x0081020400000000, 0x4081020000000000, };
constexpr board_t end_board = 0x7FFFFFFFFFFFFFFF;
constexpr int EXPAND_COUNT = 10;
constexpr reward_t C_UCB = 1;
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

struct State {
    board_t me, opp;
    score_opt eval() {
        if (any_of(lines.begin(), lines.end(), [&](const board_t &line) { return (me & line) == line; })) return 2;
        if (any_of(lines.begin(), lines.end(), [&](const board_t &line) { return (opp & line) == line; })) return -2;
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
    vector<cell_t> children() {
        vector<cell_t> res;
        board_t filled = me | opp;
        for (int x = 0; x < X; ++x) {
            board_t cell = Y * x;
            for (int y = 0; y < Y; ++y, ++cell) if (!(filled >> cell & 1)) {
                res.push_back(cell);
                break;
            }
        }
        return res;
    }
    inline State played(cell_t cell) {
        return State{opp, me | (1ULL << cell)};
    }
    State play(int row) {
        State state = this->reversed();
        board_t filled = state.me | state.opp;
        if (row == -2) return state.reversed();
        if (row == -1) return State{};
        if (0 <= row && row < X) {
            board_t cell = 1ULL << (Y * row);
            for (int y = 0; y < Y; ++y, cell <<= 1) if (!(filled & cell)) return State{state.me | cell, state.opp};
        }
        cerr << "[WARNING] unintended input: " << row << endl;
        return state;
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

struct Result {
    score_t score; cell_t cell;
    int value() {
        if (0 <= cell && cell < Y * X) return cell / Y;
        return -1;
    }

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
        const auto children = state.children();
        return memo[memo_input] = Result{state.eval_approx(), children[children.size() >> 1]};
    }

    Result res = Result{alpha, -1};
    for (const cell_t next_turn : state.children()) {
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
    optional<vector<MCTSnode>> children;
    optional<int> turn;

    reward_t add_reward(reward_t added_reward) {
        assert(-2 <= added_reward && added_reward <= 2);
        reward += (added_reward + 1) / 2;
        ++cnt;
        return added_reward;
    }
    reward_t eval() {
        score_opt h = state.eval();
        if (h) return add_reward(h.value());

        if (!children) {
            reward_t rollout_reward = rollout();
            if (cnt >= EXPAND_COUNT) expand();
            return add_reward(rollout_reward);
        } else {
            MCTSnode &largest = largest_ucb_child();
            reward_t child_reward = largest.eval();
            return add_reward(-child_reward);
        }
    }
    reward_t rollout() {
        bool is_me = true;
        State now_state = state;
        score_opt h = nullopt;
        while (!(h = now_state.eval())) {
            auto next_turns = now_state.children();
            auto next_turn = next_turns[engine() % next_turns.size()];
            now_state = now_state.played(next_turn);
            is_me = !is_me;
        }
        return (is_me ? h.value() : -h.value());
    }
    void expand() {
        children = vector<MCTSnode>{};
        for (cell_t next_turn : state.children()) {
            children.value().push_back(MCTSnode(state.played(next_turn), next_turn));
        }
    }
    MCTSnode &largest_ucb_child() {
        reward_t largest = -1e9;
        size_t largest_idx = 0;
        for (size_t i = 0; i < children.value().size(); ++i) {
            auto &child = children.value().at(i);
            if (child.cnt == 0) return child;
            reward_t ucb = (-child.reward / child.cnt) + C_UCB * sqrt(log(cnt) / cnt);
            if (largest < ucb) {
                largest = ucb;
                largest_idx = i;
            }
        }
        return children.value().at(largest_idx);
    }
    void learn() {
        clock_t deadline = TL_PER_ROUND + clock();
        int num_sims = 0;
        while ((++num_sims & 0xF) || clock() < deadline) {
            eval();
        }
        cerr << "#sim=" << num_sims << " reward=" << reward << endl;
    }
    Result choose_turn() {
        MCTSnode &largest_child = largest_ucb_child();
        for (auto &child : children.value()) {
            cerr << "turn=" << child.turn.value() << " count=" << child.cnt << " reward=" << child.reward << endl;
        }
        return Result{(score_t)largest_child.reward, largest_child.turn.value()};
    }
    
    MCTSnode(State state, cell_t turn) : state(state), reward(0), cnt(0), children(nullopt), turn(turn) {}
    MCTSnode(State state) : state(state), reward(0), cnt(0), children(nullopt), turn(nullopt) {}
    MCTSnode() : state(State{}), reward(0), cnt(0), children(nullopt), turn(nullopt) {}
};

int main(void)
{
    State state;

    int my_id; // 0 or 1 (Player 0 plays first)
    int opp_id; // if your index is 0, this will be 1, and vice versa
    cin >> my_id >> opp_id; cin.ignore();

    // game loop
    while (1) {
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
        state = state.play(opp_previous_action);

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        MCTSnode node(state);
        node.learn();
        Result result = node.choose_turn();
        cerr << "score=" << result.score << " cell=" << result.cell << endl;
        state = state.play(result.value());
        cerr << state;

        // Output a column index to drop the chip in. Append message to show in the viewer.
        cout << result.value() << endl;
    }
}