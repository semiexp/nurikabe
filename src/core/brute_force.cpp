
#include "nurikabe.h"

int nk_solver::brute_force(nk_field &field)
{
	if(field.status() == nk_field::SOLVED) return nk_field::SOLVED;
	if(field.status() & nk_field::INCONSISTENT) return nk_field::INCONSISTENT;

	int H = field.H, W = field.W;
	
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			if(field.at(i, j).value == nk_field::UNDECIDED) {
				nk_field asm_black(field);
				nk_field asm_white(field);

				asm_black.determine_black(i, j);
				solve(asm_black);

				asm_white.determine_white(i, j);
				solve(asm_white);

				int bk = brute_force(asm_black), wt = brute_force(asm_white);

				if(bk == nk_field::SOLVED) {
					field = asm_black;
					return nk_field::SOLVED;
				}
				if(wt == nk_field::SOLVED) {
					field = asm_white;
					return nk_field::SOLVED;
				}

				if(bk == nk_field::INCONSISTENT && wt == nk_field::INCONSISTENT) {
					return nk_field::INCONSISTENT;
				} else if(bk == nk_field::INCONSISTENT) {
					field = asm_white;
				} else if(wt == nk_field::INCONSISTENT) {
					field = asm_black;
				} else break;
			}
		}
	}

	return nk_field::NORMAL;
}
