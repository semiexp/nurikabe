
#include "../main.h"
#include "../core/nurikabe.h"
#include "../util/util.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <cassert>

std::vector<int> Hs, Ws;
std::vector<int*> problems;

void load_problem_file()
{
	const char problem_file[] = "testing.txt";
	std::ifstream ifs(problem_file);

	int pCnt; ifs >> pCnt;

	for(int i = 0; i < pCnt; i++) {
		int H, W;

		ifs >> H >> W;

		Hs.push_back(H);
		Ws.push_back(W);

		int* prob = new int[H * W];
		for(int j = 0; j < H * W; j++) {
			std::string in;
			ifs >> in;

			prob[j] = parse_int(in);
		}

		problems.push_back(prob);
	}
}

void run_all_problem_test()
{
	for(int i = 0; i < Hs.size(); i++) {
		nk_field fl(Hs[i], Ws[i], problems[i]);
		
		nk_solver::assumption(fl, 1);
		//nk_solver::solve(fl);

		//printf("%d\n", fl.status());
		//fl.debug(stdout);
		if(fl.status() != nk_field::SOLVED) break;
		assert(fl.status() == nk_field::SOLVED);
	}
}

void problem_test()
{
	load_problem_file();

	const int TRIAL_COUNT = 1;

	time_t start = clock();

	for(int i = 0; i < TRIAL_COUNT; i++) {
		run_all_problem_test();
	}

	time_t end = clock();

	std::cout << "Cost: " << (end - start) / (double)CLOCKS_PER_SEC << std::endl;
}
