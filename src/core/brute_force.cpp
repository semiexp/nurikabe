
#include "nurikabe.h"

static int trial = 0;

int nk_solver::brute_force(nk_field &field, int step, nk_field* answer_store[2])
{
	if(field.status() == nk_field::SOLVED) {
		if(answer_store[0]->H == 0) {
			*answer_store[0] = field;
			return nk_field::SOLVED;
		} else if(answer_store[1]->H == 0) {
			*answer_store[1] = field;
		}
		return nk_field::MULTIPLE_ANSWER;
	}

	if(field.status() & nk_field::INCONSISTENT) return nk_field::INCONSISTENT;

	if((++trial) % 1000 == 0) {
		//printf("%d\n", trial);
		//field.debug(stdout);
		//puts("");
	}

	int H = field.H, W = field.W;
	
	int ty = -1, tx = -1;

	if(step < 11) {
		// choose the candidate seriously
		int maxScore = INT_MIN;
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

					if(!flg) continue;

					nk_field asm_black(field);
					nk_field asm_white(field);

					asm_black.determine_black(i, j);
					solve(asm_black);
					asm_white.determine_white(i, j);
					solve(asm_white);

					int prog = asm_black.progress() + asm_white.progress();

					int score = 10 * prog - 200 * adjHint;

					if(maxScore < score) {
						maxScore = score;
						ty = i;
						tx = j;
					}
				}
			}
		}
	} else {
		//return nk_field::INCONSISTENT;
		int maxScore = INT_MIN;
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
							int hv = (field.at(field.root(field.id(y2, x2))).hint < 0 ? (H * W) : field.at(field.root(field.id(y2, x2))).hint) + field.at(field.root(field.id(y2, x2))).root;

							if(adjHint > hv) adjHint = hv;
						}
					}

					int score = std::min(i, H - 1 - i) + std::min(j, W - 1 - j) - 10 * adjHint;

					if(flg) {
						if(maxScore < score) {
							maxScore = score;
							ty = i;
							tx = j;
						}
					}
				}
			}
		}
	}

	if(ty == -1) return nk_field::INCONSISTENT;

	nk_field asm_black(field);
	nk_field asm_white(field);

	asm_black.determine_black(ty, tx);
	if(step < 11) assumption(asm_black, 1);
	else solve(asm_black);
	//steiner_test(asm_black);

	asm_white.determine_white(ty, tx);
	if(step < 11) assumption(asm_white, 1);
	else solve(asm_white);
	//steiner_test(asm_white);

	int bk = brute_force(asm_black, step+1, answer_store);

	if(bk & nk_field::MULTIPLE_ANSWER) return nk_field::MULTIPLE_ANSWER;

	int wt = brute_force(asm_white, step+1, answer_store);

	if((bk & nk_field::INCONSISTENT) && (wt & nk_field::INCONSISTENT)) return nk_field::INCONSISTENT;

	if((bk & nk_field::MULTIPLE_ANSWER) || (wt & nk_field::MULTIPLE_ANSWER)) {
		return nk_field::MULTIPLE_ANSWER;
	}

	if(bk == nk_field::SOLVED && wt == nk_field::SOLVED) {
		return nk_field::MULTIPLE_ANSWER;
	}

	if(bk == nk_field::SOLVED) {
		field = asm_black;
		return nk_field::SOLVED;
	}
	if(wt == nk_field::SOLVED) {
		field = asm_white;
		return nk_field::SOLVED;
	}

	asm_black.debug(stdout);
	asm_white.debug(stdout);
	field.debug(stdout);

	return nk_field::NORMAL;
}

int nk_solver::brute_force(nk_field &field)
{
	nk_field ans1, ans2;

	nk_field *sto[2] = {&ans1, &ans2};

	int ret = brute_force(field, 0, sto);

	if(ret == nk_field::SOLVED) {
		field = *(sto[0]);
	} else {
		ans1.debug(stdout);
		puts("");
		ans2.debug(stdout);
		puts("");
	}
	
	return ret;
}
