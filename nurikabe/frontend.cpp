
#include "main.h"
#include "nurikabe.h"

#include <string>
#include <iostream>

int parse_int(std::string s)
{
	int ret = 0;

	for(int i = 0; i < s.size(); i++){
		if(s[i] >= '0' && s[i] <= '9'){
			ret *= 10;
			ret += s[i] - '0';
		}else return -1;
	}

	return ret;
}

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
