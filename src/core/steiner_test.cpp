
#include "nurikabe.h"

#include <vector>
#include <queue>

int uf_root(int* uf, int p)
{
	return (uf[p] < 0 ? p : (uf[p] = uf_root(uf, uf[p])));
}

bool uf_join(int* uf, int p, int q)
{
	p = uf_root(uf, p);
	q = uf_root(uf, q);

	if(p == q) return false;

	uf[p] += uf[q];
	uf[q] = p;

	return true;
}

int nk_solver::steiner_test(nk_field &field)
{
	/* calculate the minimal spanning tree, instead of the steiner tree */

	int H = field.H, W = field.W;

	int *dist, *from;
	int hints = 0, blacks = 0;
	dist = new int[H * W];
	from = new int[H * W];

	std::queue<int> Q;
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			dist[field.id(i, j)] = -1;
			if(field.at(i, j).value == nk_field::BLACK) {
				++blacks;

				dist[field.id(i, j)] = 0;
				from[field.id(i, j)] = field.id(i, j);
				Q.push(field.id(i, j));
			}
			if(field.at(i, j).hint > 0) hints += field.at(i, j).hint;
		}
	}

	while(!Q.empty()) {
		int bas = Q.front();
		int y = bas / W, x = bas % W;
		Q.pop();

		for(int i = 0; i < 4; i++) {
			int y2 = nk_field::ay[i] + y, x2 = nk_field::ax[i] + x;

			if(field.range(y2, x2) && field.at(y2, x2).value != nk_field::WHITE && dist[field.id(y2, x2)] == -1) {
				dist[field.id(y2, x2)] = dist[bas] + 1;
				from[field.id(y2, x2)] = from[bas];

				Q.push(field.id(y2, x2));
			}
		}
	}
	
	std::vector<std::pair<int, std::pair<int, int> > > edges;

	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			int p1 = field.id(i, j);
			if(i > 0) {
				int p2 = field.id(i-1, j);
				if(dist[p1] >= 0 && dist[p2] >= 0) {
					edges.push_back(std::make_pair(dist[p1] + dist[p2], std::make_pair(from[p1], from[p2])));
				}
			}
			if(j > 0) {
				int p2 = field.id(i, j-1);
				if(dist[p1] >= 0 && dist[p2] >= 0) {
					edges.push_back(std::make_pair(dist[p1] + dist[p2], std::make_pair(from[p1], from[p2])));
				}
			}
		}
	}

	std::sort(edges.begin(), edges.end());

	delete [] dist;
	delete [] from;

	int *union_find = new int[H * W];
	for(int i = 0; i < H*W; i++) union_find[i] = -1;

	int unions = 1;
	int cost = 0;
	int max_cost = 0;

	for(int i = 0; i < edges.size(); i++) {
		if(uf_join(union_find, edges[i].second.first, edges[i].second.second)) {
			cost += edges[i].first;
			if(max_cost < edges[i].first) max_cost = edges[i].first;
			++unions;
		}
	}

	delete [] union_find;

	if(unions < blacks) {
		return field.t_status |= nk_field::INCONSISTENT;
	}

	// Minimal spanning tree gives 3-approximation (require formal proof) for the minimal number of the required black cells

	int approx_steiner_cost = (cost == 0 ? 0 : ((cost + 2) / 3));
	if(approx_steiner_cost < max_cost) approx_steiner_cost = max_cost;

	if(approx_steiner_cost + blacks + hints > H * W)
		return field.t_status |= nk_field::INCONSISTENT;

	return nk_field::NORMAL;
}
