
#include "main.h"
#include "nurikabe.h"
#include "util.h"

#include <string>
#include <iostream>
#include <ctime>

void report_field(nk_field &field, int tq, double cost)
{
	if(field.status() & nk_field::INCONSISTENT){
		puts("inconsistent problem");
	}else if(field.status() & nk_field::SOLVED){
		printf("successfully solved (%.3fs)\n", cost);
		switch(tq)
		{
		case 0:
			puts("no trial-and-error technique was used");
			break;
		case 1:
			puts("1-step trial-and-error techniques were used");
			break;
		}
		puts("");

		FILE* fp = stdout;
		for(int i = 0; i < field.board_height(); i++){
			for(int j = 0; j < field.board_width(); j++){
				if(field.cell_hint(i, j) > 0){
					fprintf(fp, "%2d", field.cell_hint(i, j));
				}else if(field.cell_value(i, j) == nk_field::BLACK){
					fprintf(fp, "##");
				}else if(field.cell_value(i, j) == nk_field::WHITE){
					fprintf(fp, "..");
				}

				if(j != field.board_width() - 1) fprintf(fp, " ");
			}

			fprintf(fp, "\n");
		}
	}
}

void pencilbox_frontend()
{
	int H, W;
	time_t t1, t2;

	std::cin >> H >> W;

	if(!(0 < H && H < 120) || !(0 < W && W < 120)){
		puts("error: too large board");
		return;
	}

	int *vals = new int[H * W];

	for(int i = 0; i < H * W; i++){
		std::string in;

		if(std::cin.eof()){
			puts("error: unexpected eof (too short problem description)");
			return;
		}

		std::cin >> in;

		vals[i] = parse_int(in);
	}

	puts("problem loaded...\n");

	t1 = clock();

	nk_field field(H, W, vals);

	t2 = clock();

	if(field.status() != nk_field::NORMAL){
		report_field(field, 0, (double)(t2 - t1) / CLOCKS_PER_SEC);

		return;
	}

	nk_solver::assumption(field, 1);

	t2 = clock();

	if(field.status() != nk_field::NORMAL){
		report_field(field, 1, (double)(t2 - t1) / CLOCKS_PER_SEC);

		return;
	}

	puts("this problem is too difficult for this solver.");
}
