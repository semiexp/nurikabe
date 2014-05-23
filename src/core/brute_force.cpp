
#include "nurikabe.h"

static int step = 0;

int nk_solver::brute_force(nk_field &field)
{
	if(field.status() == nk_field::SOLVED) return nk_field::SOLVED;
	if(field.status() & nk_field::INCONSISTENT) return nk_field::INCONSISTENT;

	if((++step) % 100 == 0) {
		field.debug(stdout);
		puts("");
	}

	int H = field.H, W = field.W;
	
	int mHint = H * W + 1;
	int ty = -1, tx = -1;

	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			if(field.at(i, j).value == nk_field::UNDECIDED) {
				int adjHint = H * W;
				bool flg = false;

				for(int k = 0; k < 4; k++) {
					int y2 = i + nk_field::ay[k];
					int x2 = j + nk_field::ax[k];

					if(field.range(y2, x2) && field.at(y2, x2).value == nk_field::WHITE) {
						flg = true;
						int hv = field.at(field.root(field.id(y2, x2))).hint + field.at(field.root(field.id(y2, x2))).root;

						if(adjHint > hv) adjHint = hv;
					}
				}

				if(flg) {
					if(mHint > adjHint) {
						mHint = adjHint;
						ty = i;
						tx = j;
					}
				}
			}
		}
	}

	if(ty == -1) return nk_field::INCONSISTENT;

	nk_field asm_black(field);
	nk_field asm_white(field);

	asm_black.determine_black(ty, tx);
	//assumption(asm_black, 1);
	solve(asm_black);

	asm_white.determine_white(ty, tx);
	solve(asm_white);
	//assumption(asm_white, 1);

	int bk = brute_force(asm_black), wt = brute_force(asm_white);

	if((bk & nk_field::INCONSISTENT) && (wt & nk_field::INCONSISTENT)) return nk_field::INCONSISTENT;

	if(bk == nk_field::SOLVED) {
		field = asm_black;
		return nk_field::SOLVED;
	}
	if(wt == nk_field::SOLVED) {
		field = asm_white;
		return nk_field::SOLVED;
	}

	/*
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
	*/
	asm_black.debug(stdout);
	asm_white.debug(stdout);
	field.debug(stdout);

	return nk_field::NORMAL;
}
