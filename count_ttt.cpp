/*
 * count_ttt: count possible states of a tic-tac-toe game
 * Assume X goes first
 * Empty:0 X:1 O:2
 */
#include <iostream>

using board = int;

// masks
const board row = 077;
const board col = 030303;
const board diag[] = {0601403,0031460};
const board odd = 0x55555;
const board even = 0xaaaaa;

// see if it is a reachable state
bool reachable(const board &x)
{
	
		// winner of the game, if any
	int win = 0;

	// one cannot win twice, nor after his opponents' victory
	for (int i = 0; i < 3; ++i)
	{
		int j = row & (x >> (i * 6));
		j &= (j >> 2);
		j &= (j >> 2) & 3;
		if (j)
		if (!win)
			win = j;	
		else
			return false;
	}

	for (int i = 0; i < 3; ++i)
	{
		int j = col & (x >> (i * 2));
		j &= (j >> 6);
		j &= (j >> 6) & 3;
		if (j)
		if (!win)
			win = j;
		else
			return false;
	}
	
	for (int i = 0; i < 2; ++i)
	{
		int j = diag[i] & x;
		j &= (j >> 8 - 4 * i);
		j &= (j >> 8 - 4 * i);
		j = (j >> 4*i) & 3;
		if (j)
		if (!win)
			win = j;
		else
			return false;
	}
	
	// check count(X) - count(O) = {0,1}
	int xcount = 0;
	for (int i = 0; i < 9; ++i)
	{
		// tmp is the piece on the tile
		int tmp = (x >> i * 2) & 3;
		// if tmp == 2 then -1 else add tmp
		xcount += ((tmp == 2) ? -1 : tmp);
	}

	// if x win, win = 1, xcount = 1, xor -> 0
	// if o win, win = 2, xcount = 0, xor -> 2
	switch (win)
	{
		case 1: return xcount == 1;
		case 2: return xcount == 0;
		default:
				return xcount == 1 | xcount == 0;
	}
}

int main()
{
	// 19683 = 3^9
	const int total = 19683;
	int ans = 0;

	// convert the number to a board
	for (int i = 0; i < total; ++i)
	{
		board tb = 0;
		auto cp = i;
		for (int j = 0; j < 9; ++j)
		{
			tb <<= 2;
			tb |= cp % 3;
			cp /= 3;
		}
		ans += reachable(tb);
	}

	std::cout << ans << std::endl;
	return 0;
}
