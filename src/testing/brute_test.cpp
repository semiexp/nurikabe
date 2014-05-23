
#include "../main.h"
#include "../core/nurikabe.h"

#include <ctime>

const int probH = 5;
const int probW = 5;

const int problem[probH * probW] = {
	 4, -1, -1, -1,  4,
	-1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1,
	 4, -1, -1, -1,  4
};

/*
http://puzzletokyo.up.seesaa.net/image/Habanero_merged.pdf
Habanero [5] (very difficult; even with 3-steps trial-and-error, this problem couldn't be solved)
 */
const int probH2 = 10;
const int probW2 = 10;

const int problem2[probH2 * probW2] = {
	 6, -1, -1, -1,  9, -1, -1, -1, -1,  6,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 8, -1, -1, -1,  6, -1, -1, -1, -1,  4,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 6, -1, -1, -1, 10, -1, -1, -1, -1,  9,
};

/*
http://puzzletokyo.seesaa.net/article/384672397.html
*/
const int probH3 = 10;
const int probW3 = 10;

const int problem3[probW3 * probW3] = {
	20, -1, -1, -1, -1, -1, -1, -1, -1, 14,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1,  5, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	18, -1, -1, -1, -1, -1, -1, -1, -1, 19,
};

/*
http://www.nikoli.co.jp/ja/puzzles/nurikabe.html
*/
const int probH4 = 7;
const int probW4 = 7;

const int problem4[probH4 * probH4] = {
	-1,  3, -1, -1, -1,  1, -1,
	-1, -1, -1, -1, -1, -1, -1,
	 2, -1, -1,  1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,
	-1,  1, -1, -1,  2, -1, -1,
	-1, -1,  2, -1, -1, -1, -1,
	 1, -1, -1, -1,  1, -1,  6,
};

void brute_test()
{
	nk_field field(probH2, probW2, (int*) problem2);

	time_t start = clock();

	puts("Solve:");
	field.debug(stdout);
	fflush(stdout);

	printf("%d\n", nk_solver::brute_force(field));
	field.debug(stdout);

	time_t end = clock();

	printf("Cost: %.3f[s]\n", (end - start) / (double)CLOCKS_PER_SEC);
}
