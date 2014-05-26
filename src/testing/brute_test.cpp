
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

const int probH5 = 7;
const int probW5 = 7;

const int problem5[probH5 * probH5] = {
	 9, -1, -1, -1, -1, -1,  9,
	-1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,
	 9, -1, -1, -1, -1, -1,  9,
};

/*
janko.at #688
 */
const int probH6 = 20;
const int probW6 = 20;

const int problem6[probH6 * probH6] = {
	-1, -1, 20, -1, 10, -1, -1, 1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, 2,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, 1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, 1, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, -1,
	20, -1, 10, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, 20, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, 1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, -1,
	20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1,
	10, -1, -1, -1, -1, -1, 2, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 20, -1, 10, -1, -1,
};

/*
janko.at #687
 */
const int probH7 = 17;
const int probW7 = 17;

const int problem7[probH7 * probH7] = {
	-1, -1, -1, 9, -1, 3, -1, -1, -1, -1, -1, 3, -1, 13, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	8, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 4,
	-1, -1, -1, -1, -1, -1, -1, 4, -1, 3, -1, -1, -1, -1, -1, -1, -1,
	5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1,
	-1, -1, -1, 3, -1, -1, -1, -1, 25, -1, -1, -1, -1, 3, -1, -1, -1,
	-1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3,
	-1, -1, -1, -1, -1, -1, -1, 3, -1, 14, -1, -1, -1, -1, -1, -1, -1,
	3, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 3,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, 2, -1, 2, -1, -1, -1, -1, -1, 2, -1, 2, -1, -1, -1,
};

/*
janko.at #669
 */
const int probH8 = 14;
const int probW8 = 24;

const int problem8[probH8 * probW8] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, -1, -1, -1, -1, 4, -1, -1, -1, -1, 16, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, 4, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, 4,
	-1, -1, -1, -1, -1, -1, -1, 64, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 8, -1, -1, -1, -1, 1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	2, -1, 2, -1, 2, -1, 2, -1, -1, -1, -1, -1, -1, -1, 4, -1, 32, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, 4, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, 8, -1, -1, -1, -1, 4, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

/*
janko.at 29
*/
const int probH9 = 10;
const int probW9 = 10;

const int problem9[probH9 * probW9] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 37, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 36, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

/*
janko.at 753
*/
const int probH10 = 10;
const int probW10 = 10;

const int problem10[probH10 * probW10] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  8, -1, -1, -1, -1, -1, -1, 30, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 28, -1, -1, -1, -1, -1, -1, 15, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


void brute_test()
{
	nk_field field(probH10, probW10, (int*) problem10);

	time_t start = clock();

	puts("Solve:");
	nk_solver::assumption(field, 1);
	field.debug(stdout, true);
	fflush(stdout);

	printf("%d\n", nk_solver::brute_force(field));
	field.debug(stdout);

	time_t end = clock();

	printf("Cost: %.3f[s]\n", (end - start) / (double)CLOCKS_PER_SEC);
}
