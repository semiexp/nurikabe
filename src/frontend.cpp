
#include "main.h"
#include "nurikabe.h"
#include "util.h"

#include <string>
#include <iostream>

void solver_frontend()
{
	int H, W;
	
	std::cin >> H >> W;

	nk_field field(H, W);

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			std::string in;
			std::cin >> in;

			int val = parse_int(in);

			if(val > 0) field.set_hint(i, j, val);
		}

	nk_solver::assumption(field, 4);

	field.debug(stdout);

	if(field.status() & nk_field::INCONSISTENT){
		puts("inconsistent problem");
	}else if(field.status() & nk_field::SOLVED){
		puts("successfully solved");
	}else{
		puts("too difficult to solve");
	}
}
