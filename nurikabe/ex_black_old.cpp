
#include "nurikabe.h"
#include "graph.h"

#include <queue>
#include <algorithm>

bool nk_solver::expand_black_naive_test(nk_field &field, int y, int x)
{
	int* cells = new int[field.H * field.W];
	bool* visited = new bool[field.H * field.W];

	for(int i=0;i<field.H;i++)
		for(int j=0;j<field.W;j++){
			visited[field.id(i, j)] = false;

			switch(field.at(i, j).value)
			{
			case nk_field::UNDECIDED:
				cells[field.id(i, j)] = (i==y && j==x) ? -1 : 0;
				break;
			case nk_field::BLACK:
				cells[field.id(i, j)] = 1;
				break;
			case nk_field::WHITE:
				cells[field.id(i, j)] = -1;
				break;
			}
		}

	std::vector<std::pair<int, int> > cands;
	for(int i=0;i<4;i++){
		cands.push_back(expand_black_naive_sub(y + nk_field::ay[i], x + nk_field::ax[i], field.H, field.W, cells, visited));
	}

	int cnt = 0;
	for(int i=0;i<4;i++) if(cands[i].second > 0) ++cnt;

	delete [] cells;
	delete [] visited;

	if(cnt >= 2) return true;
	
	return false;
}

std::pair<int, int> nk_solver::expand_black_naive_sub(int y, int x, int H, int W, int* cells, bool* visited)
{
	if(y < 0 || x < 0 || y >= H || x >= W || visited[y * W + x] || cells[y * W + x] == -1) return std::make_pair(0, 0);

	visited[y * W + x] = true;
	std::pair<int, int> ret(1, cells[y*W+x] == 1 ? 1 : 0);

	for(int i=0;i<4;i++){
		std::pair<int, int> tmp = expand_black_naive_sub(y + nk_field::ay[i], x + nk_field::ax[i], H, W, cells, visited);

		ret.first += tmp.first;
		ret.second += tmp.second;
	}

	return ret;
}

int nk_solver::expand_black_naive(nk_field &field)
{
	/*
	Naive algorithm: O(W^2*H^2)
	*/
	int H = field.H, W = field.W;

	int ret = nk_field::NORMAL;

	for(int i=0;i<H;i++)
		for(int j=0;j<W;j++)
			if(field.at(i, j).value == nk_field::UNDECIDED){
				//assume there to be white
				if(expand_black_naive_test(field, i, j)){
					ret |= field.determine_black(i, j);
				}
			}

	return ret;
}
