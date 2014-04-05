
#include "nurikabe.h"
#include "graph.h"

#include <queue>
#include <algorithm>

int nk_solver::expand_black(nk_field &field)
{
	/*
	Fast algorithm: O(WH)

	note: already separated -- inconsistent
	*/

	int H = field.H, W = field.W;

	int ret = nk_field::NORMAL; int iLast=0;

	int *id = new int[H*W];

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			if(field.at(i, j).value == nk_field::WHITE) id[i*W+j] = -1;
			else id[field.id(i, j)] = iLast++;
		}

	int *val = new int[iLast];

	int ip = 0;
	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			if(id[i * W + j] >= 0){
				val[ip++] = (field.at(i, j).value == nk_field::BLACK) ? 1 : 0;
			}
		}

	naive_allocator al;
	separated_graph<int, naive_allocator> G(iLast, iLast * 4, &al, val);
	std::vector<int> sep;

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++) if(id[i * W + j] >= 0){
			if(i < H-1 && id[(i+1)*W+j] >= 0) G.add_edge(id[i*W+j], id[(i+1)*W+j]);
			if(j < W-1 && id[i*W+j+1] >= 0) G.add_edge(id[i*W+j], id[i*W+j+1]);
		}

	// graph must be connected! 

	G.init();

	int p_count = 0;

	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			int p = field.id(i, j);
			if(id[p] == -1 || !G.is_root(id[p])) continue;

			if(G.get_union_value(id[p]) > 0) ++p_count;
		}
	}

	if(p_count >= 2){
		delete [] id;
		delete [] val;

		return field.t_status |= nk_field::INCONSISTENT;
	}

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			if(field.at(i, j).value == nk_field::UNDECIDED){
				G.query(id[field.id(i, j)], sep);

				int nz = 0;
				for(int k = 0; k < sep.size(); k++) if(sep[k] > 0) ++nz;

				if(nz >= 2){
					ret |= field.determine_black(i, j);
				}
			}
		}

	delete [] id;
	delete [] val;

	return ret;
}

