
#include "../main.h"
#include "../core/nurikabe.h"

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

void csp_test()
{
	nk_field field(probH2, probW2, (int*) problem2);

	nk_solver::solve_csp(field);

}
