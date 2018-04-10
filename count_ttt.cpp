/*
 * count_ttt: count possible states of a tic-tac-toe game
 * Assume X goes first
 * Empty:0 X:1 O:2
 */
#include <iostream>
#include <vector>

using board = std::vector<short>;

int get_lines(const board &x, int c)
{
	int ans = 0;
	for (int i = 0; i < 3; ++i)
	{
		// columns
		if (x[0 + i] == c && x[3 + i] == c && x[6 + i] == c)
			++ans;
		// rows
		if (x[0 + i * 3] == c && x[1 + i * 3] == c && x[2 + i * 3] == c)
			++ans;
	}
	if (x[0] == c && x[4] == c && x[8] == c)
		++ans;
	if (x[2] == c && x[4] == c && x[6] == c)
		++ans;
	return ans;
}

int get_count(const board& x, int c)
{
	int ans = 0;
	for (auto i : x)
		ans += (i == c);
	return ans;
}

// see if it is a reachable state
bool reachable(const board &x)
{
	int xcount = get_count(x, 1);
	int ocount = get_count(x, 2);
	int x_lines = get_lines(x, 1);
	int o_lines = get_lines(x, 2);

	return x_lines + o_lines <= 1 && xcount >= ocount + x_lines && xcount <= ocount + 1 - o_lines;
}

int main()
{
	// 19683 = 3^9
	const int total = 19683;
	int ans = 0;

	// convert the number to a board
	for (int i = 0; i < total; ++i)
	{
		board tb;
		auto cp = i;
		for (int j = 0; j < 9; ++j)
		{
			tb.push_back(cp % 3);
			cp /= 3;
		}
		ans += reachable(tb);
	}

	std::cout << ans << std::endl;
	return 0;
}
