#include <iostream>
#include <queue>
#include <vector>
#include <cstring>
#include <cmath>
#include <limits>

using std::abs;

using board = short[9];

enum class Direction {
	up = -1, down = 1, left = 2, right = -2, none = 0
};

int mh[9][9] = {{4,3,2,3,2,1,2,1,0},{0,1,2,1,2,3,2,3,4},{1,0,1,2,1,2,3,2,3},{2,1,0,3,2,1,4,3,2},{1,2,3,0,1,2,1,2,3},{2,1,2,1,0,1,2,1,2},{3,2,1,2,1,0,3,2,1},{2,3,4,1,2,3,0,1,2},{3,2,3,2,1,2,1,0,1}};

class State {
	public:
		/// numbers
		board nums;
		/// steps taken to reach current State
		int steps;
		/// previous State in the search tree, the direction means how to return to it
		Direction prev;

		State(const board nums) : steps(0), prev(Direction::none)
		{
			std::memcpy(this->nums, nums, 9 * sizeof(short));
		}

		// cost estimated by manhattan distance
		int mht() const noexcept
		{
			int ans = 0;
			for (int i = 0; i < 9; ++i)
				ans += mh[nums[i]][i];
			return ans;
		}


		/// displaced blocks
		int displaced() const noexcept
		{
			int ans = 0;
			for (short i = 0; i < 9; ++i)
				ans += !!(mh[nums[i]][i]);
			return ans;
		}

		/// create next State out of this one
		/// no bound check
		/// \param dir Direction of the empty block
		State gen_next(Direction dir) const noexcept
		{
			int offset = 0;
			switch (dir) {
				case Direction::up : offset = -3; break;
				case Direction::down : offset = 3; break;
				case Direction::left : offset = -1; break;
				case Direction::right : offset = 1;
			}

			short null_place = 0;
			for (int i = 0; i < 9; ++i)
			if (!nums[i])
			{
				null_place = i;
				break;
			}

			State next{this->nums};
			next.steps = this->steps + 1;
			next.prev = static_cast<Direction>(static_cast<int>(dir));

			std::swap(next.nums[null_place], next.nums[null_place + offset]);
			
			return next;
		}

		/// check if it is the target state
		bool reached() const noexcept
		{
			for (short i = 0; i < 9; ++i)
				if ((i + 1) % 9 != nums[i])
					return false;
			return true;
		}

		/// return possible moves
		std::vector<State> next_states() noexcept;

};

std::vector<State> State::next_states() noexcept
{
	std::vector<State> result;

	int null_place = 0;

	// find which piece of the board is empty
	for (int i = 0; i < 9; ++i)
		if (!nums[i])
		{
			null_place = i;
			break;
		}

	// available Directions
	// optimization: one cannot return to its preceding state
	if (null_place > 2 && prev != Direction::down)
		result.push_back(this->gen_next(Direction::up));
	if (null_place % 3 > 0 && prev != Direction::right)
		result.push_back(this->gen_next(Direction::left));
	if (null_place % 3 < 2 && prev != Direction::left)
		result.push_back(this->gen_next(Direction::right));
	if (null_place < 6 && prev != Direction::up)
		result.push_back(this->gen_next(Direction::down));
	
	return result;
}

// Used to print the board
std::ostream& operator<<(std::ostream& ost, const State& st)
{
	std::cout << "STEP " << st.steps << ", MHT" << st.mht() << '\n';
	for (short i = 0; i < 9; ++i)
	{
		ost << st.nums[i] << ' ';
		if (i % 3 == 2)
			ost << '\n';
	}
	return ost;
}

// used to compare states
// if less()(a, b) is true, b is in front of a in the priority queue
struct less1 {
	bool operator()(const State a, const State b) {
		return a.steps + a.mht() > b.steps + b.mht();
	}
};

struct less2 {
	bool operator()(const State a, const State b) {
		return a.steps + a.mht() + a.displaced()> b.steps + b.mht() + b.displaced();
	}
};

template<class less>
using StateList = std::priority_queue<State, std::vector<State>, less>;

template<class less>
double solve(const board initial_board)
{
	StateList<less> search_list;
	// push it into the searching list
	search_list.push(State{initial_board});

	while (!search_list.empty())
	{
		State now = search_list.top();
		search_list.pop();

		//std::cout << "==========================" << std::endl;

		//std::cout << now;

		//char x;
		//std::cin >> x;

		// check if it is the final state
		if (now.reached())
		{
			return now.steps;
		}

		// abort if there are too many steps (to tackle unsolvable cases)
		//, 40 is an arbitrary number
		if (now.steps > 40)
		{
			return std::numeric_limits<long double>::max();
		}

		auto next_states = now.next_states();
		for (auto state : now.next_states())
		{
			search_list.push(state);
		}
	}
}

const int fact[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320};

// decode: convert a number in 0..9!-1 to a board
inline void decode(board x, int no) noexcept
{
	std::vector<short> numbers({0, 1, 2, 3, 4, 5, 6, 7, 8});
	for (int i = 8; i > 0; --i)
	{
		auto num = no / fact[i];
		x[i] = numbers[num]; 
		numbers.erase(numbers.begin() + num);
		no %= fact[i];
	}
	x[0] = numbers[0];
}

// check if a board state is not solvable
bool unsolvable(int no)
{
	int flag = 0;
	for (int i = 8; i > 0; --i)
	{
		auto num = no / fact[i];
		flag += i - num;
		no %= fact[i];
	}
	return (flag & 1);
}

int main()
{
	board initial_board;
	for (int i = 0; i < 362880; ++i)
	{
		// skip states that will never reach the end state
		if (unsolvable(i))
		{
			continue;
		}
		// convert the number into the board
		decode(initial_board, i);
		// solve the board using different evaluation functions
		auto ans1 = solve<less1>(initial_board);
		auto ans2 = solve<less2>(initial_board);

		if (ans1 != ans2)
		{
			// if the second search gives suboptimal result,
			// print the board
			std::cout << State(initial_board) << std::endl;
			std::cout << ans1 << ' ' << ans2 << std::endl;
		}
	}
	return 0;
}