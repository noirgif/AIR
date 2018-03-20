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
			{	
				ans += abs(i / 3 - nums[i] / 3) + abs(i % 3 - nums[i] % 3);
			}
			return ans;
		}


		/// displaced blocks
		int displaced() const noexcept
		{
			int ans = 0;
			for (short i = 0; i < 9; ++i)
				ans += (nums[i] != i);
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
			if (nums[i] == 8)
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
				if (nums[i] != i)
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

	for (int i = 0; i < 9; ++i)
		if (nums[i] == 8)
		{
			null_place = i;
			break;
		}

	/// available Directions
	std::vector<Direction> dirs;
	if (null_place > 2)
		dirs.push_back(Direction::up);
	if (null_place % 3 > 0)
		dirs.push_back(Direction::left);
	if (null_place % 3 < 2)
		dirs.push_back(Direction::right);
	if (null_place < 6)
		dirs.push_back(Direction::down);

	for (auto dir : dirs)
		if (dir != prev)
			result.push_back(this->gen_next(dir));
	
	return result;
}

std::ostream& operator<<(std::ostream& ost, const State& st)
{
	std::cout << "STEP " << st.steps << ", MHT" << st.mht() << '\n';
	for (short i = 0; i < 9; ++i)
	{
		ost << st.nums[i];
		if (i % 3 == 2)
			ost << '\n';
	}
	return ost;
}

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

#ifndef LARGER_ESTIMATE
std::priority_queue<State, std::vector<State>, less1> search_list;
#else
std::priority_queue<State, std::vector<State>, less2> search_list;
#endif

int main()
{

	// enter the initial State
	board initial_board;
	for (int i = 0; i < 9; ++i)
	{
		short a;
		std::cin >> a;
		initial_board[i] = a;
	}

	// push it into the searching list
	search_list.push(State{initial_board});

	long double ans = std::numeric_limits<long double>::max();

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
			ans = now.steps;
			// break out of the loop
			break;
		}

		// warn if there are too many steps, 50 is an arbitrary number
		if (now.steps > 50)
		{
			std::cout << "Too many steps" << std::endl;
			continue;
		}

		auto next_states = now.next_states();
		for (auto state : now.next_states())
		{
			search_list.push(state);
		}
	}

	std::cout << "Steps taken: " << int(ans) << std::endl;
	return 0;
}