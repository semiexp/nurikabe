
#include "../main.h"
#include "../core/nurikabe.h"
#include "../util/util.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

void show_for_pencilbox(nk_field &field, std::ostream &os)
{
	os << field.board_height() << std::endl
	   << field.board_width()  << std::endl;

	for(int i = 0; i < field.board_height(); i++) {
		for(int j = 0; j < field.board_width(); j++) {
			if(field.cell_hint(i, j) > 0) {
				os << field.cell_hint(i, j) << " ";
			}else if(field.cell_value(i, j) == nk_field::BLACK) {
				os << "# ";
			}else if(field.cell_value(i, j) == nk_field::WHITE) {
				os << "+ ";
			}
		}

		os << std::endl;
	}
}

void report_field(nk_field &field, int tq, double cost, std::string infn)
{
	if(field.status() & nk_field::INCONSISTENT){
		std::cout << "Inconsistent problem" << std::endl;

		return;
	}

	if(!(field.status() & nk_field::SOLVED)) {
		std::cout << "Fatal error: the status is neither 'INCONSISTENT' nor 'SOLVED'" << std::endl;
		
		return;
	}

	std::cout << "Successfully solved (" << cost << "s)" << std::endl;

	switch(tq)
	{
	case 0:
		std::cout << "No trial-and-error technique was used." << std::endl;
		break;
	case 1:
		std::cout << "1-step trial-and-error techniques were used." << std::endl;
		break;
	case 2:
		std::cout << "2-steps trial-and-error techniques were used." << std::endl;
		break;
	case 3:
		std::cout << "3-steps trial-and-error techniques were used." << std::endl;
		break;
	}
	std::cout << std::endl;

	for(int i = 0; i < field.board_height(); i++){
		for(int j = 0; j < field.board_width(); j++){
			if(field.cell_hint(i, j) > 0){
				std::cout << std::setw(2) << field.cell_hint(i, j) << " ";
			}else if(field.cell_value(i, j) == nk_field::BLACK){
				std::cout << "## ";
			}else if(field.cell_value(i, j) == nk_field::WHITE){
				std::cout << ".. ";
			}

		}

		std::cout << std::endl;
	}

	std::cout << std::endl;

	std::string outfn = infn + std::string(".report.txt");
	std::ofstream ofs(outfn);

	if(!ofs) {
		std::cout << "Error: Tried to write the solution to " << outfn << " but couldn't open the file." << std::endl;

		return;
	}

	show_for_pencilbox(field, ofs);

	std::cout << "Wrote the solution to " << outfn << "." << std::endl;
}

void pencilbox_frontend(int argc, char** argv)
{
	int H, W;
	time_t t1, t2;

	std::string in_name;

	if(argc >= 2){
		in_name = argv[1];
	}else{
		std::cout << "Input file name: ";
		std::getline(std::cin, in_name);

		//chop '\r', '\n'
		while(in_name.size() > 0 && (in_name[in_name.size() - 1] == '\r' || in_name[in_name.size() - 1] == '\n')) {
			in_name.pop_back();
		}
	}

	std::ifstream ifs(in_name);

	if(!ifs) {
		std::cout << "Error: Couldn't open file \"" << in_name << "\"" << std::endl;
		std::cout << "Press any key to continue...";
		get_onekey();
		return;
	}

	ifs >> H >> W;

	if(!(0 < H && H < 120) || !(0 < W && W < 120)){
		std::cout << "Error: Too large board" << std::endl;
		std::cout << "Press any key to continue...";
		get_onekey();
		return;
	}

	int *vals = new int[H * W];

	for(int i = 0; i < H * W; i++){
		std::string in;

		if(std::cin.eof()){
			std::cout << "Error: unexpected eof (too short problem description)" << std::endl;
			std::cout << "Press any key to continue...";
			get_onekey();
			return;
		}

		ifs >> in;

		vals[i] = parse_int(in);
	}

	std::cout << "Problem successfully loaded..." << std::endl << std::endl;

	t1 = clock();

	nk_field field(H, W, vals);

	t2 = clock();

	if(field.status() != nk_field::NORMAL){
		report_field(field, 0, (double)(t2 - t1) / CLOCKS_PER_SEC, in_name);

		goto wait_key_for_exit;
	}

	nk_solver::assumption(field, 1);

	t2 = clock();

	if(field.status() != nk_field::NORMAL){
		report_field(field, 1, (double)(t2 - t1) / CLOCKS_PER_SEC, in_name);

		goto wait_key_for_exit;
	}

	std::cout << "This problem couldn't be solved with 1-step trial-and-error techniques." << std::endl;

	char ct;
	do{
		std::cout << "Proceed with 2-steps trial-and-error? [y/n]";
		ct = get_onekey();
		std::cout << ct << std::endl;
	}while(ct != 'y' && ct != 'n');

	if(ct == 'n') goto giveup;

	t1 = clock();

	nk_solver::assumption(field, 2);

	t2 = clock();

	if(field.status() != nk_field::NORMAL){
		report_field(field, 2, (double)(t2 - t1) / CLOCKS_PER_SEC, in_name);

		goto wait_key_for_exit;
	}

	std::cout << "This problem couldn't be solved with 2-step trial-and-error techniques." << std::endl;

	do{
		std::cout << "Proceed with 3-steps trial-and-error? [y/n]";
		ct = get_onekey();
		std::cout << ct << std::endl;
	}while(ct != 'y' && ct != 'n');

	if(ct == 'n') goto giveup;

	t1 = clock();

	nk_solver::assumption(field, 3);

	t2 = clock();

	if(field.status() != nk_field::NORMAL){
		report_field(field, 3, (double)(t2 - t1) / CLOCKS_PER_SEC, in_name);

		goto wait_key_for_exit;
	}

giveup:
	std::cout << "This problem is too difficult for this solver." << std::endl;

wait_key_for_exit:
	std::cout << "Press any key to continue...";
	fflush(stdout);
	get_onekey();
}
