
#include "nurikabe.h"
#include "graph.h"
#include "../util/util.h"

#include <queue>
#include <algorithm>

nk_solver::vector3 nk_solver::expand_white_dfs(int y, int x, int H, int W, bool* visited, nk_field &field)
{
	if(y < 0 || y >= H || x < 0 || x >= W || visited[field.id(y, x)] || field.at(y, x).value == nk_field::BLACK)
		return vector3(0, 0, 0);

	visited[field.id(y, x)] = true;

	vector3 ret(1, field.at(y, x).value == nk_field::WHITE ? 1 : 0, field.at(y, x).hint > 0 ? field.at(y, x).hint : 0, field.at(y, x).hint > 0 ? 1 : 0);

	ret += expand_white_dfs(y-1, x, H, W, visited, field);
	ret += expand_white_dfs(y+1, x, H, W, visited, field);
	ret += expand_white_dfs(y, x-1, H, W, visited, field);
	ret += expand_white_dfs(y, x+1, H, W, visited, field);

	return ret;
}

int nk_solver::expand_white(nk_field &field)
{
	int H = field.H, W = field.W;
	naive_allocator al;

	bool *visited = (bool*) al.allocate(H * W * sizeof(bool));

	for(int i = 0; i < H * W; i++) visited[i] = false;

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			if(!visited[field.id(i, j)] && field.at(i, j).value != nk_field::BLACK){
				vector3 unit = expand_white_dfs(i, j, H, W, visited, field);

				//printf("%d %d: %d %d %d\n", i, j, unit.x, unit.y, unit.z);

				if(unit.z + (unit.w > 0 ? ((unit.w + 1) / 3) : 0) > unit.x) {
					al.release(visited);
					return field.t_status |= nk_field::INCONSISTENT;
				}
			}
		}

	al.release(visited);

	int *cstate, *uroot, *unext, *cid, idl = 0; vector3 *cvalue, *uvalue, *adj_total;
	cstate = (int*) al.allocate(H * W * sizeof(int));
	uroot = (int*) al.allocate(H * W * sizeof(int));
	unext = (int*) al.allocate(H * W * sizeof(int));
	cid = (int*) al.allocate(H * W * sizeof(int));
	cvalue = (vector3*) al.allocate(H * W * sizeof(vector3));
	uvalue = (vector3*) al.allocate(H * W * sizeof(vector3));
	adj_total = (vector3*) al.allocate(H * W * sizeof(vector3));

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			int p = field.id(i, j);

			if(field.at(p).value == nk_field::BLACK || field.at(field.root(field.id(i, j))).hint == -field.at(field.root(field.id(i, j))).root){
				cstate[p] = -2;
				cid[p] = -1;
			}else{
				cstate[p] = -1;
				if(field.at(i, j).hint > 0) cstate[p] = field.id(i, j);

				cid[p] = idl++;
				for(int k = 0; k < 4; k++){
					int y2 = i + nk_field::ay[k];
					int x2 = j + nk_field::ax[k];

					if(field.range(y2, x2) && field.at(field.root(field.id(y2, x2))).hint > 0){
						cstate[p] = field.root(field.id(y2, x2));
					}
				}

				cvalue[p] = uvalue[p] = 
					vector3(
						1,
						(field.at(p).value == nk_field::WHITE) ? 1 : 0,
						(field.at(p).hint > 0) ? field.at(p).hint : 0
					);
			}

			uroot[p] = -1;
			unext[p] = p;
		}

	std::queue<int> Q;

	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			if(cstate[field.id(i, j)] == -2 || uroot[field.id(i, j)] != -1) continue;

			int rt = field.id(i, j);

			uroot[rt] = rt;

			Q.push(rt);

			while(!Q.empty()){
				int y = Q.front() / W, x = Q.front() % W;
				Q.pop();

				for(int k = 0; k < 4; k++){
					int y2 = y + nk_field::ay[k];
					int x2 = x + nk_field::ax[k];

					int p = field.id(y2, x2);

					if(!field.range(y2, x2) || uroot[p] != -1 || cstate[rt] != cstate[p]) continue;

					uroot[p] = rt;
					uvalue[rt] = uvalue[rt] + cvalue[p];
					std::swap(unext[rt], unext[p]);

					Q.push(p);
				}
			}

		}
	}

	for(int i = 0; i < H * W; i++) adj_total[i] = uvalue[i];

	std::vector<std::pair<std::pair<int, int>, int> > adjs;
	/*
	 ((root 1, root 2), cell 1)
	 connecting cell 1 and one of the cells belonging to root 2
	 */

	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			int p = field.id(i, j);

			if(cstate[p] == -2) continue;

			if(i < H - 1 && cstate[field.id(i+1, j)] != -2 && cstate[field.id(i+1, j)] != cstate[p] && 
				(cstate[field.id(i+1, j)] == -1 || cstate[p] == -1)){
				adjs.push_back(std::make_pair(std::make_pair(
					uroot[p],
					uroot[field.id(i+1, j)]),
					p
				));
				adjs.push_back(std::make_pair(std::make_pair(
					uroot[field.id(i+1, j)],
					uroot[p]),
					field.id(i+1, j)
				));
			}

			if(j < W - 1 && cstate[field.id(i, j+1)] != -2 && cstate[field.id(i, j+1)] != cstate[p] && 
				(cstate[field.id(i, j+1)] == -1 || cstate[p] == -1)){
				adjs.push_back(std::make_pair(std::make_pair(
					uroot[p],
					uroot[field.id(i, j+1)]),
					p
				));
				adjs.push_back(std::make_pair(std::make_pair(
					uroot[field.id(i, j+1)],
					uroot[p]),
					field.id(i, j+1)
				));
			}
		}
	}

	std::sort(adjs.begin(), adjs.end());
	adjs.erase(std::unique(adjs.begin(), adjs.end()), adjs.end());
	
	int eid = 0;

	for(int i = 0; i < adjs.size(); i++) {
		if(i == 0 || adjs[i].first != adjs[i-1].first){
			// adjs[i].first.second is now adjacent to adjs[i].first.first
			adj_total[adjs[i].first.first] += uvalue[adjs[i].first.second];
			adj_total[adjs[i].first.first] += vector3(1 << 16, 1 << 16, 1 << 16);
			//printf("add %d: %d %d %d\n", adjs[i].first.first, adjs[i].first.first, adjs[i].first.second, adjs[i].second);
			++eid;
		}
	}
	
	separated_graph <vector3, naive_allocator> G(idl + eid, idl * 4, &al);

	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			int p = field.id(i, j);

			if(cstate[p] == -2) continue;

			if(i < H - 1 && cstate[field.id(i+1, j)] == cstate[p]){
				G.add_edge(cid[field.id(i, j)], cid[field.id(i+1, j)]);
			}

			if(j < W - 1 && cstate[field.id(i, j+1)] == cstate[p]){
				G.add_edge(cid[field.id(i, j)], cid[field.id(i, j+1)]);
			}
		}
	}

	std::vector<std::pair<std::pair<int, int>, int> >::iterator iter = adjs.begin(), iter2, end = adjs.end();
	vector3 *vals = (vector3*) al.allocate((idl + eid) * sizeof(vector3));

	for(int i = 0; i < idl + eid; i++) vals[i] = vector3(0, 0, 0);

	for(int i = 0; i < H * W; i++){
		if(cid[i] >= 0) vals[cid[i]] = cvalue[i];
	}

	eid = 0;

	while(iter != end) {
		iter2 = iter;

		while(iter2 != end && iter->first == iter2->first) ++iter2;

		// (* TODO: do something for [iter, iter2) *)

		//printf("%d is based on %d, %d\n", idl + eid, iter->first.second, iter->first.first);
		vals[idl + eid] = adj_total[iter->first.second] + -(vector3(1 << 16, 1 << 16, 1 << 16) + uvalue[iter->first.first]);

		if(cstate[iter->first.first] != -1 && vals[idl + eid].x >= (1 << 16)) {
			vals[idl + eid] = vector3(uvalue[iter->first.second].x, uvalue[iter->first.second].y + (1 << 16), 1 << 16);
		}

		for(; iter != iter2; ++iter){
			G.add_edge(cid[iter->second], idl + eid);
		}

		++eid;
	}

	G.set_value(vals);
	//for(int i=0;i<idl+eid;i++) printf("%d: %d %d %d\n", i, vals[i].x, vals[i].y, vals[i].z);
	G.init();

	mini_vector<vector3, 4> sto;

	int ret = nk_field::NORMAL;

	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			if(cid[field.id(i, j)] == -1) continue;

			if(G.is_root(cid[field.id(i, j)])){
				vector3 uval = G.get_union_value(cid[field.id(i, j)]);

				//if(uval.y < (1 << 16) && uval.z < (1 << 16) && uval.y > uval.z){
				if(uval.z < (1 << 16) && (uval.y & 0xffff) > uval.z) {
					field.t_status |= nk_field::INCONSISTENT;
					ret |= nk_field::INCONSISTENT;

					goto release;
				//}else if(uval.x < (1 << 16) && uval.z < (1 << 16) && uval.z > uval.x){
				}else if(uval.x < (1 << 16) && (uval.z & 0xffff) > uval.x) {
					field.t_status |= nk_field::INCONSISTENT;
					ret |= nk_field::INCONSISTENT;

					goto release;
				}else if(uval.z == 0 && (uval.x & 0xffff) >= 2){
					field.t_status |= nk_field::INCONSISTENT;
					ret |= nk_field::INCONSISTENT;

					goto release;
				}
			}

			G.query(cid[field.id(i, j)], sto);
			for(int k = 0; k < sto.size(); k++){
				vector3 tmp = sto[k];
				//if(tmp.y < (1 << 16) && tmp.z < (1 << 16) && tmp.y > tmp.z){
				if(tmp.z < (1 << 16) && (tmp.y & 0xffff) > tmp.z) {
					ret |= field.determine_white(i, j);
				//}else if(tmp.x < (1 << 16) && tmp.z < (1 << 16) && tmp.z > tmp.x){
				}else if(tmp.x < (1 << 16) && (tmp.z & 0xffff) > tmp.x){
					ret |= field.determine_white(i, j);
				}else if(tmp.z == 0 && (tmp.x & 0xffff) >= 2){
					ret |= field.determine_white(i, j);
				}
			}
		}
	}

release:
	al.release(cstate);
	al.release(uroot);
	al.release(unext);
	al.release(cid);
	al.release(cvalue);
	al.release(uvalue);
	al.release(adj_total);
	al.release(vals);

	return ret;
}
