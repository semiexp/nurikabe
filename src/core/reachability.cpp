
#include "nurikabe.h"
#include "graph.h"

#include <queue>
#include <algorithm>

int nk_solver::check_reachability(nk_field &field)
{
	//dijkstra

	int H = field.H, W = field.W;
	int *mx_rem = new int[H*W];
	bool *adj = new bool[H*W];
	std::priority_queue<std::pair<int, int> > Q;

	for(int i=0;i<H*W;i++){ mx_rem[i] = -1; adj[i] = false; }

	int ret = nk_field::NORMAL;

	for(int i=0;i<H;i++)
		for(int j=0;j<W;j++){
			/*
			if(field.at(i, j).value == nk_field::WHITE){
				int root = field.root(field.id(i, j));
				int hint = field.at(root).hint;
				if(hint < 0) continue;

				int sz = -field.at(root).root;
				for(int k=0;k<4;k++){
					int y2 = i+nk_field::ay[k], x2 = j+nk_field::ax[k];
					if(field.range(y2, x2) && field.at(y2, x2).value == nk_field::UNDECIDED){
						Q.push(std::make_pair(hint - sz, y2*W+x2));
						adj[y2*W+x2] = true;
					}
				}
			}
			*/
			if(field.at(i, j).hint < 0) continue;

			int cid = field.id(i, j);
			int cbeg = cid;

			int rem = field.at(cid).hint + field.at(cid).root;
			if(rem <= 0) continue;

			int cand1=-1, cand2=-1;

			do{
				int y = cid/W, x = cid%W;

				cid = field.at(cid).next;
				adj[cid] = true;

				for(int k=0;k<4;k++){
					int y2 = y+nk_field::ay[k], x2 = x+nk_field::ax[k];
					if(field.range(y2, x2) && field.at(y2, x2).value == nk_field::UNDECIDED){
						int nid = field.id(y2, x2);
						if(cand1 == -1) cand1 = nid;
						else if(cand1 != nid){
							if(cand2 == -1) cand2 = nid;
							else if(cand2 != nid) cand2 = -2;
						}
						Q.push(std::make_pair(rem, nid));
						adj[y2*W+x2] = true;
					}
				}
			}while(cid != cbeg);

			if(rem == 1 && cand2 >= 0){
				int ya=cand1/W, xa=cand1%W, yb=cand2/W, xb=cand2%W;

				if((ya-yb==1 || ya-yb==-1) && (xa-xb==1 || xa-xb==-1)){
					if(field.at(ya, xb).value == nk_field::UNDECIDED){
						ret |= field.determine_black(ya, xb);
					}
					if(field.at(yb, xa).value == nk_field::UNDECIDED){
						ret |= field.determine_black(yb, xa);
					}
				}
			}
		}

	while(!Q.empty()){
		std::pair<int, int> top = Q.top(); Q.pop();

		if(mx_rem[top.second] != -1) continue;
		mx_rem[top.second] = top.first;
		int y = top.second/W, x = top.second%W;

		if(top.first == 1) continue;

		for(int i=0;i<4;i++){
			int y2 = y+nk_field::ay[i], x2 = x+nk_field::ax[i];

			if(field.range(y2, x2) && !adj[y2 * W + x2] && field.at(y2, x2).value != nk_field::BLACK){
				Q.push(std::make_pair(top.first-1, y2*W+x2));
			}
		}
	}

	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			//printf("%3d ", mx_rem[i*W+j]);

			if(mx_rem[field.id(i, j)] != -1) {
				if(field.at(i, j).value == nk_field::WHITE && field.at(field.root(field.id(i, j))).hint < 0) {
					int root = field.root(field.id(i, j));
					mx_rem[root] = std::max(mx_rem[root], mx_rem[field.id(i, j)]);
				}
				continue;
			}

			if(field.at(i, j).value == nk_field::UNDECIDED){
				ret |= field.determine_black(i, j);
			}

			if(field.at(i, j).value == nk_field::WHITE && field.at(field.root(field.id(i, j))).hint < 0){
				delete [] mx_rem;
				delete [] adj;

				return field.t_status |= nk_field::INCONSISTENT;
			}
		}
		//puts("");
	}
	
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			int id = field.id(i, j);
			if(field.at(id).value == nk_field::WHITE && field.at(id).root < 0 && field.at(id).hint < 0) {
				if(-field.at(id).root > mx_rem[id] + 1) {
					delete [] mx_rem;
					delete [] adj;

					return field.t_status |= nk_field::INCONSISTENT;
				}
			}
		}
	}

	delete [] mx_rem;
	delete [] adj;

	return ret;
}
