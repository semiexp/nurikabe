
#include "../core/nurikabe.h"
#include "../main.h"
#include <iostream>
#include <ctime>

int prob[250 * 250];

void consecutive_test()
{
	// input: stdin

	int pCount;
	std::cin >> pCount;

	int success = 0;
	int inconsistent = 0;
	int difficult = 0;

	for(int i = 1; i <= pCount; i++) {
		int H, W;
		std::cin >> H >> W;

		bool flg = false;

		for(int j = 0; j < H*W; j++) {
			std::cin >> prob[j];
			if(prob[j] == -2) flg = true;
		}

		if(flg) {
			printf("Problem #%d: Problems including '?'s are not supported\n", i);
			fprintf(stderr, "Problem #%d: Problems including '?'s are not supported\n", i);
			continue;
		}

		int steps = 1;

		time_t start = clock();

		nk_field field(H, W, prob);
		nk_solver::assumption(field, 1);

		if(field.status() == nk_field::NORMAL) {
			steps = 2;
			nk_solver::assumption(field, 2);
		}

		time_t end = clock();
		double dur = (end - start) / (double)CLOCKS_PER_SEC;

		if(field.status() == nk_field::INCONSISTENT) {
			printf("Problem #%d: Inconsistent problem (%.3f[s])\n", i, dur);
			fprintf(stderr, "Problem #%d: Inconsistent problem (%.3f[s])\n", i, dur);

			++inconsistent;
		} else if(field.status() == nk_field::SOLVED) {
			printf("Problem #%d: Successfully solved[%d] (%.3f[s])\n", i, steps, dur);
			fprintf(stderr, "Problem #%d: Successfully solved[%d] (%.3f[s])\n", i, steps, dur);

			++success;
		} else {
			printf("Problem #%d: Too difficult (%.3f[s])\n", i, dur);
			fprintf(stderr, "Problem #%d: Too difficult (%.3f[s])\n", i, dur);

			++difficult;
		}

	}

	printf("Success: %d\nInconsistent: %d\nDifficult: %d\nTotal: %d\n", success, inconsistent, difficult, pCount);
}
