
#include "nurikabe.h"
#include "graph.h"

#include <queue>
#include <algorithm>

bool nk_solver::expand_white_naive_test(nk_field &field, int y, int x)
{
	int* cells = new int[field.H * field.W];
	bool* visited = new bool[field.H * field.W];

	for(int i=0;i<field.H;i++)
		for(int j=0;j<field.W;j++){
			visited[field.id(i, j)] = false;

			switch(field.at(i, j).value)
			{
			case nk_field::UNDECIDED:
				cells[field.id(i, j)] = (i==y && j==x) ? -2 : 0;
				break;
			case nk_field::BLACK:
				cells[field.id(i, j)] = -2;
				break;
			case nk_field::WHITE:
				cells[field.id(i, j)] = (field.at(i, j).hint > 0 ? field.at(i, j).hint : -1);
				break;
			}
		}

	// available white cell, sum of hints
	std::vector<std::pair<int, int> > cands;
	for(int i=0;i<4;i++){
		cands.push_back(expand_white_naive_sub(y + nk_field::ay[i], x + nk_field::ax[i], field.H, field.W, cells, visited));
	}

	int cnt = 0;
	for(int i=0;i<4;i++) if(cands[i].first < cands[i].second) cnt = 1;

	delete [] cells;
	delete [] visited;

	if(cnt >= 1) return true;
	
	return false;
}

std::pair<int, int> nk_solver::expand_white_naive_sub(int y, int x, int H, int W, int* cells, bool* visited)
{
	if(y < 0 || x < 0 || y >= H || x >= W || visited[y * W + x] || cells[y * W + x] == -2) return std::make_pair(0, 0);

	visited[y * W + x] = true;
	std::pair<int, int> ret(1, cells[y*W+x] == -1 ? 0 : cells[y*W+x]);

	for(int i=0;i<4;i++){
		std::pair<int, int> tmp = expand_white_naive_sub(y + nk_field::ay[i], x + nk_field::ax[i], H, W, cells, visited);

		ret.first += tmp.first;
		ret.second += tmp.second;
	}

	return ret;
}

int nk_solver::expand_white_naive(nk_field &field)
{
	int H = field.H, W = field.W;

	int ret = nk_field::NORMAL;

	for(int i=0;i<H;i++)
		for(int j=0;j<W;j++)
			if(field.at(i, j).value == nk_field::UNDECIDED){
				//assume there to be white
				if(expand_white_naive_test(field, i, j)){
					ret |= field.determine_white(i, j);
				}
			}

	return ret;
}

int nk_solver::expand_white(nk_field &field)
{
	struct vector3
	{
		int x, y, z;

		vector3(){ x = y = z = 0; }
		vector3(int x, int y, int z) : x(x), y(y), z(z) {}

		inline vector3 operator+(const vector3& v){ return vector3(x + v.x, y + v.y, z + v.z); }
		inline vector3 operator-(){ return vector3(-x, -y, -z); }
	};

	int H = field.H, W = field.W;

	int *st = new int[H*W], *root = new int[H*W], *size = new int[H*W], *next = new int[H*W], *id = new int[H*W];
	vector3 *val = new vector3[H*W], *vsum = new vector3[H*W];

	for(int i=0;i<H;i++)
		for(int j=0;j<W;j++){
			if(field.at(i, j).value == nk_field::BLACK){
				st[field.id(i, j)] = -2;
			}else{
				st[field.id(i, j)] = -1;

				if(field.at(i, j).hint > 0) st[field.id(i, j)] = field.id(i, j);

				for(int k=0;k<4;k++){
					int y2 = i+nk_field::ay[k], x2 = j+nk_field::ax[k];
					if(field.range(y2, x2) && field.at(field.root(field.id(y2, x2))).hint > 0){
						st[field.id(i, j)] = field.root(field.id(y2, x2));
						continue;
					}
				}

				val[field.id(i, j)] = vector3(1, (field.at(i, j).value == nk_field::WHITE ? 1 : 0), (field.at(i, j).hint > 0 ? field.at(i, j).hint : 0));
				vsum[field.id(i, j)] = val[field.id(i, j)];
			}

			root[field.id(i, j)] = -1;
			size[field.id(i, j)] = 0;
			next[field.id(i, j)] = field.id(i, j);
		}

	std::queue<int> Q;

	for(int i=0;i<H;i++)
		for(int j=0;j<W;j++) if(root[field.id(i, j)] == -1 && st[field.id(i, j)] != -2){
			//fill with BFS

			int p = field.id(i, j);
			Q.push(p);
			root[p] = p;
			++size[p];

			while(!Q.empty()){
				int y = Q.front() / W, x = Q.front() % W; Q.pop();

				for(int k=0;k<4;k++){
					int y2 = y + nk_field::ay[k], x2 = x + nk_field::ax[k];
					if(field.range(y2, x2) && root[field.id(y2, x2)] == -1 && st[field.id(y2, x2)] == st[p]){
						Q.push(field.id(y2, x2));
						root[field.id(y2, x2)] = p;
						vsum[p] = vsum[p] + vsum[field.id(y2, x2)];
						std::swap(next[p], next[field.id(y2, x2)]);
						++size[p];
					}
				}
			}
		}


	/*
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++) printf("%2d/%2d ", root[i*W+j], st[i*W+j]);
		puts("");
	}
	*/

	naive_allocator al;
	std::vector<vector3> sep;
	std::vector<std::pair<int, int> > adj;
	int ret = nk_field::NORMAL;
	const int INF = 1<<20;

	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++) if(root[field.id(i, j)] == field.id(i, j)){
			if(st[field.id(i, j)] == -1){
				// global

				// (* TODO *)
			}else{
				// local

				// assign ids
				adj.clear();

				int p=field.id(i, j), il=0;
				do{
					id[p] = il++;

					for(int k=0;k<4;k++){
						int y2 = p/W + nk_field::ay[k], x2 = p%W + nk_field::ax[k];
						if(field.range(y2, x2) && st[field.id(y2, x2)] == -1){
							adj.push_back(std::make_pair(root[field.id(y2, x2)], id[p]));
						}
					}

					p = next[p];
				}while(p != field.id(i, j));

				std::sort(adj.begin(), adj.end());
				int uc = 0;
				for(int i=0;i<adj.size();i++) if(i==0 || adj[i].first != adj[i-1].first) ++uc;

				separated_graph<vector3, naive_allocator> G((int)il + uc, (int)(il + uc) * 4, &al);
				vector3 *vert = new vector3[il + uc]; // (number of cells, number of white cells, value of hint)

				p = field.id(i, j);
				do{
					vert[id[p]] = val[p];
					for(int k=0;k<4;k++){
						int y2 = p/W + nk_field::ay[k], x2 = p%W + nk_field::ax[k];
						if(!field.range(y2, x2)) continue;

						if(root[field.id(y2, x2)] == field.id(i, j)){
							G.add_edge(id[p], id[field.id(y2, x2)]);
						}
					}

					p = next[p];
				}while(p != field.id(i, j));

				uc = il - 1;
				for(int k=0;k<adj.size();k++){
					if(k == 0 || adj[k].first != adj[k-1].first){
						++uc;
						vert[uc] = vsum[adj[k].first];
						//vert[uc].z = INF;
					}

					G.add_edge(uc, adj[k].second);
				}

				G.set_value(vert);
				//for(int k=0;k<=uc;k++) printf("%d %d %d\n", vert[k].x, vert[k].y, vert[k].z);
				G.init();

				p = field.id(i, j);
				for(int i=0;i<il;i++){
					//std::vector<vector3> sep = G.query(i);
					G.query(i, sep);
					for(int j=0;j<sep.size();j++){
						vector3 tmp = sep[j];
						//if((tmp.z < INF && tmp.x < tmp.z) || (tmp.x >= 2 && tmp.z == 0) || (tmp.y > tmp.z)){
						if(tmp.x < tmp.z){
							//field.debug(stdout);
							//printf("%d %d %d %d %d %d\n", i, p/W, p%W, tmp.x, tmp.y, tmp.z);
							ret |= field.determine_white(p / W, p % W);
						}
					}

					p = next[p];
				}
			}
		}
	}

	delete [] st;
	delete [] root;
	delete [] size;
	delete [] next;
	delete [] id;
	delete [] val;
	delete [] vsum;

	return nk_field::NORMAL;
}
