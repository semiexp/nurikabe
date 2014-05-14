
#include "nurikabe.h"

#include "../../nanocsp/src/core/NanoCSP.h"
#include "../../nanocsp/src/core/Expr.hpp"

using namespace NanoCSP;

int nk_solver::solve_csp(nk_field &field)
{
	// (* TODO: write the solver *)

	int H = field.H, W = field.W;
	NCSolver sol;

	std::vector<std::vector<NCBool> > B;
	std::vector<std::vector<NCInt> > Id;
	// Cell status; B[i][j] = true iff (i, j) is a black cell

	for(int i = 0; i < H; i++) {
		std::vector<NCBool> Bi;
		std::vector<NCInt> Idi;
		for(int j = 0; j < W; j++) {
			Bi.push_back(NCBool(sol));
			Idi.push_back(NCInt(sol, 0, H * W));
		}

		B.push_back(Bi);
		Id.push_back(Idi);
	}

	std::vector<std::vector<NCInt> > Eh;
	std::vector<std::vector<NCBool> > Eh2;
	// horizontal edges: Eh[i][j] = edge from (i, j) to (i, j+1)

	for(int i = 0; i < H; i++) {
		std::vector<NCInt> Ehi;
		std::vector<NCBool> Eh2i;
		for(int j = 0; j < W-1; j++) {
			Ehi.push_back(NCInt(sol, -10, 10)); // (* TODO *)
			Eh2i.push_back(NCBool(sol));
		}

		Eh.push_back(Ehi);
		Eh2.push_back(Eh2i);
	}

	std::vector<std::vector<NCInt> > Ev;
	std::vector<std::vector<NCBool> > Ev2;

	// vertical edges: Eh[i][j] = edge from (i, j) to (i+1, j)
	for(int i = 0; i < H-1; i++) {
		std::vector<NCInt> Evi;
		std::vector<NCBool> Ev2i;
		for(int j = 0; j < W; j++) {
			Evi.push_back(NCInt(sol, -10, 10)); // (* TODO *)
			Ev2i.push_back(NCBool(sol));
		}

		Ev.push_back(Evi);
		Ev2.push_back(Ev2i);
	}

	
	// Value of an edge must be 0 unless it connects white cells
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			// horizontal
			if(j < W - 1) {
				//sol.satisfy((Eh[i][j] != 0) >>= (!B[i][j] && !B[i][j+1]));
				sol.satisfy((B[i][j] || B[i][j+1]) >>= (Eh[i][j] == 0));
			}

			//vertical
			if(i < H - 1) {
				sol.satisfy((Ev[i][j] != 0) >>= (!B[i][j] && !B[i+1][j]));
			}
		}
	}

	// White Cell flow
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			int target = (field.at(i, j).hint > 0 ? (field.at(i, j).hint - 1) : -1);

			std::vector<NCInt> in, out;

			if(i > 0) {
				in.push_back(Ev[i-1][j]);
			}

			if(j > 0) {
				in.push_back(Eh[i][j-1]);
			}

			if(i < H-1) {
				out.push_back(Ev[i][j]);
			}

			if(j < W-1) {
				out.push_back(Eh[i][j]);
			}

			if(in.size() == 0 && out.size() == 0) return nk_field::NORMAL;

			// sum(in) - target == sum(out)

			if(in.size() == 0 && out.size() == 2) {
				sol.satisfy(B[i][j] || (-target == out[0] + out[1]));
			}

			if(in.size() == 1 && out.size() == 1) {
				sol.satisfy(B[i][j] || (in[0] == (out[0] + target)));
			}

			if(in.size() == 2 && out.size() == 0) {
				sol.satisfy(B[i][j] || (target == in[0] + in[1]));
			}

			if(in.size() == 1 && out.size() == 2) {
				sol.satisfy(B[i][j] || ((in[0] - target) == out[0] + out[1]));
			}

			if(in.size() == 2 && out.size() == 1) {
				sol.satisfy(B[i][j] || (in[0] + in[1] == (out[0] + target)));
			}

			if(in.size() == 2 && out.size() == 2) {
				sol.satisfy(B[i][j] || (in[0] + in[1] == (out[0] + target) + out[1]));
			}

			for(int p = 0; p < in.size() + out.size(); ++p ){
				if(field.at(i, j).hint > 0) {
					if(p < in.size()) sol.satisfy(in[p] >= 0);
					if(p >= in.size()) sol.satisfy(out[p - in.size()] <= 0);
				}

				for(int q = p+1; q < in.size() + out.size(); ++q) {
					if(p < in.size() && q < in.size()) {
						sol.satisfy(in[p] >= 0 || in[q] >= 0);
					}
					
					if(p < in.size() && q >= in.size()) {
						sol.satisfy(in[p] >= 0 || out[q - in.size()] <= 0);
					}

					if(p >= in.size() && q < in.size()) {
						sol.satisfy(out[p - in.size()] <= 0 || in[q] >= 0);
					}

					if(p >= in.size() && q >= in.size()) {
						sol.satisfy(out[p - in.size()] <= 0 || out[q - in.size()] <= 0);
					}
				}
			}
		}
	}

	// Avoid 2*2 black cells
	for(int i = 0; i < H - 1; i++) {
		for(int j = 0; j < W-1; j++) {
			sol.satisfy(!B[i][j] || !B[i][j+1] || !B[i+1][j] || !B[i+1][j+1]);
		}
	}

	// Adjacent white cells must have the same value
	int wid = 0;
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			if(field.at(i, j).hint > 0) {
				sol.satisfy(Id[i][j] == wid++);
				sol.satisfy(!B[i][j] || !B[i][j]);
			}

			if(i < H - 1) {
				sol.satisfy((!B[i][j] && !B[i+1][j]) >>= (Id[i][j] == Id[i+1][j]));
			}

			if(j < W - 1) {
				sol.satisfy((!B[i][j] && !B[i][j+1]) >>= (Id[i][j] == Id[i][j+1]));
			}
		}
	}

	// Black Cell flow
	const int ax[4] = {-1, 0, 0, 1};
	const int ay[4] = {0, -1, 1, 0};

	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			/*
			if(i < H - 1) {
				sol.satisfy((B[i][j] && B[i+1][j] && Ev2[i][j]) >>= (Id[i][j] < Id[i+1][j]));
				sol.satisfy((B[i][j] && B[i+1][j] && !Ev2[i][j]) >>= (Id[i][j] > Id[i+1][j]));
			}

			if(j < W - 1) {
				sol.satisfy((B[i][j] && B[i][j+1] && Eh2[i][j]) >>= (Id[i][j] < Id[i][j+1]));
				sol.satisfy((B[i][j] && B[i][j+1] && !Eh2[i][j]) >>= (Id[i][j] > Id[i][j+1]));
			}

			std::vector<NCBool> out;

			if(i > 0) out.push_back((B[i][j] && B[i-1][j] && !Ev2[i-1][j]).trans(&sol));
			if(j > 0) out.push_back((B[i][j] && B[i][j-1] && !Eh2[i][j-1]).trans(&sol));
			if(i < H - 1) out.push_back((B[i][j] && B[i+1][j] && Ev2[i][j]).trans(&sol));
			if(j < W - 1) out.push_back((B[i][j] && B[i][j+1] && Eh2[i][j]).trans(&sol));

			if(out.size() == 2) {
				sol.satisfy(!B[i][j] || Id[i][j] == 0 || (out[0] || out[1]));
			}
			if(out.size() == 3) {
				sol.satisfy(!B[i][j] || Id[i][j] == 0 || (out[0] || out[1] || out[2]));
			}
			if(out.size() == 4) {
				sol.satisfy(!B[i][j] || Id[i][j] == 0 || (out[0] || out[1] || out[2] || out[3]));
			}
			*/

			int y = i;
			int x = j;

			std::vector<NCBool> cand;

			for(int k = 0; k < 4; k++) {
				int y2 = i + ay[k];
				int x2 = j + ax[k];

				if(y2 < 0 || x2 < 0 || y2 >= H || x2 >= W) continue;

				NCBool Ei(sol);
				sol.satisfy(Ei == (!B[y][x] || (B[y2][x2] && Id[y][x] > Id[y2][x2])));

				cand.push_back(Ei);
			}

			NCBool is_zero(sol);
			sol.satisfy(is_zero == (Id[y][x] == 0));

			if(cand.size() == 2) {
				sol.satisfy(is_zero || cand[0] || cand[1]);
			}
			if(cand.size() == 3) {
				sol.satisfy(is_zero || cand[0] || cand[1] || cand[2]);
			}
			if(cand.size() == 4) {
				sol.satisfy(is_zero || cand[0] || cand[1] || cand[2] || cand[3]);
			}
		}
	}

	for(int i = 0; i < H*W; i++) {
		for(int j = i+1; j < H*W; j++) {
			int y1 = i/W, x1 = i%W;
			int y2 = j/W, x2 = j%W;

			sol.satisfy((B[y1][x1] && B[y2][x2]) >>= (Id[y1][x1] != 0 || Id[y2][x2] != 0));
		}
	}

	if(sol.solve()) {
		for(int i = 0; i < H; i++) {
			for(int j = 0; j < W; j++) {
				if(field.at(i, j).hint > 0) printf("%2d/%2d ", field.at(i, j).hint, sol.GetIntValue(Id[i][j]));
				else printf("%s/%2d ", (sol.GetBoolValue(B[i][j]) ? "##" : ".."), sol.GetIntValue(Id[i][j]));
			}
			puts("");
		}

		/*
		for(int i = 0; i < H; i++) {
			for(int j = 0; j < W; j++) {
				printf("%s ", (sol.GetBoolValue(B[i][j]) ? "##" : ".."));
				if(j != W-1) printf("%2d-> ", sol.GetIntValue(Eh[i][j]));
			}
			printf("\n");

			if(i != H-1) {
				for(int j = 0; j < W; j++) printf("%2d      ", sol.GetIntValue(Ev[i][j]));
			}
			printf("\n");
		}
		*/
	}else puts("Couldn't solve the problem");

	return nk_field::NORMAL;
}
