
#include <vector>
#include <algorithm>

template <class Abelian, class Allocator>
class separated_graph
{
	struct edge
	{
		int dest;
		edge *next;
	};
	
	Allocator *al; char *bas;
	int V, E; int *left, *right, *lowlink;
	edge **nodes, *pool; int pLast;
	Abelian* value;
	bool *vis; int *root_pos;

	int *root; Abelian *sol;

	void add_edge_sub(int s, int d);
	void dfs(int p, int root, int rtop, int& id);
	int find_child(int p, int dis);

public:
	separated_graph(){ al = NULL; V = 0; nodes = NULL; root = NULL; sol = NULL; }
	separated_graph(int V_, int E_, Allocator *al_);
	separated_graph(int V_, int E_, Allocator *al_, Abelian *val_);
	~separated_graph();

	void set_value(Abelian *val_){ for(int i=0;i<V;i++) value[i] = val_[i]; }
	void add_edge(int s, int d){ add_edge_sub(s, d); add_edge_sub(d, s); }

	void init();

	void query(int p, std::vector<Abelian> &ret);
	std::vector<Abelian> query(int p);

	bool is_root(int p){ return root_pos[p] == p; }
	Abelian get_union_value(int p){ return value[root_pos[p]]; }
};

class naive_allocator
{
public:
	void* allocate(int sz)
	{
		return (void*) new char[sz];
	}

	void release(void* pt)
	{
		delete [] ((char*) pt);
	}
};

template <class Abelian, class Allocator>
separated_graph<Abelian, Allocator>::separated_graph(int V_, int E_, Allocator* al_, Abelian* val_)
{
	V = V_; E = E_ * 2;
	al = al_; pLast = 0;

	bas = (char*) al->allocate(E * sizeof(edge) + V * (sizeof(edge*) + sizeof(Abelian) + sizeof(int) * 4 + sizeof(bool)));

	pool = (edge*) bas;
	nodes = (edge**) (bas + E * sizeof(edge));
	value = (Abelian*) ((char*)nodes + V * sizeof(edge*));
	left = (int*) ((char*)value + V * sizeof(Abelian));
	right = left + V;
	lowlink = right + V;
	root_pos = lowlink + V;
	vis = (bool*) ((char*)root_pos + V * sizeof(int));
	root = NULL; sol = NULL;

	for(int i = 0; i < V; i++){
		nodes[i] = NULL;
		value[i] = val_[i];
	}
}

template <class Abelian, class Allocator>
separated_graph<Abelian, Allocator>::separated_graph(int V_, int E_, Allocator* al_)
{
	V = V_; E = E_ * 2;
	al = al_; pLast = 0;

	bas = (char*) al->allocate(E * sizeof(edge) + V * (sizeof(edge*) + sizeof(Abelian) + sizeof(int) * 4 + sizeof(bool)));

	pool = (edge*) bas;
	nodes = (edge**) (bas + E * sizeof(edge));
	value = (Abelian*) ((char*)nodes + V * sizeof(edge*));
	left = (int*) ((char*)value + V * sizeof(Abelian));
	right = left + V;
	lowlink = right + V;
	root_pos = lowlink + V;
	vis = (bool*) ((char*)root_pos + V * sizeof(int));
	root = NULL; sol = NULL;

	for(int i = 0; i < V; i++){
		nodes[i] = NULL;
	}
}


template <class Abelian, class Allocator>
separated_graph<Abelian, Allocator>::~separated_graph()
{
	if(!al) return;

	al->release(bas);
	if(root) al->release(root);
	if(sol) al->release(sol);
}

template <class Abelian, class Allocator>
void separated_graph<Abelian, Allocator>::add_edge_sub(int s, int d)
{
	edge *e = &(pool[pLast++]);
	e->dest = d;
	e->next = nodes[s];
	nodes[s] = e;
}

template <class Abelian, class Allocator>
void separated_graph<Abelian, Allocator>::dfs(int p, int root, int rtop, int& id)
{
	if(vis[p]) return;
	vis[p] = true;

	left[p] = id++;
	lowlink[p] = left[p];
	root_pos[p] = rtop;

	edge *n_beg = NULL, *n_last = NULL;

	for(edge* nx=nodes[p]; nx!=NULL; nx=nx->next){
		if(vis[nx->dest]){
			if(nx->dest != root) lowlink[p] = std::min(lowlink[p], left[nx->dest]);
			//nx->dest = ~nx->dest;
			//instead, forget this edge completely
		}else{
			dfs(nx->dest, p, rtop, id);
			lowlink[p] = std::min(lowlink[p], lowlink[nx->dest]);
			value[p] = value[p] + value[nx->dest];

			//printf("%d -> %d\n", p, nx->dest);
			if(n_beg == NULL) n_beg = n_last = nx;
			else{
				n_last->next = nx;
				n_last = nx;
			}
		}
	}

	if(n_last) n_last->next = NULL;
	nodes[p] = n_beg;

	right[p] = ++id;
}

template <class Abelian, class Allocator>
void separated_graph<Abelian, Allocator>::init()
{
	int id = 0;
	//bool *vis = (bool*) al->allocate(V * sizeof(bool*));
	for(int i = 0; i < V; i++){
		vis[i] = false;
		root_pos[i] = -1;
	}

	for(int i = 0; i < V; i++) if(!vis[i]){
		dfs(i, -1, i, id);
	}

	int mx_deg = 0;
	for(int i = 0; i < V; i++){
		int deg = 0;
		for(edge *nx=nodes[i]; nx!=NULL; nx=nx->next) ++deg;

		if(mx_deg < deg) mx_deg = deg;
	}

	root = (int*) al->allocate((mx_deg+1) * sizeof(int));
	sol = (Abelian*) al->allocate((mx_deg+1) * sizeof(Abelian));

	//for(int i=0;i<V;i++) printf("%d: [%d, %d) %d\n", i, left[i], right[i], lowlink[i]);
}

template <class Abelian, class Allocator>
int separated_graph<Abelian, Allocator>::find_child(int p, int dis)
{
	if(!(left[p] <= dis && dis < right[p])) return -1;

	int idx=0;
	for(edge* nx = nodes[p]; nx != NULL; nx = nx->next, ++idx){
		if(nx->dest >= 0){
			if(left[nx->dest] <= dis && dis < right[nx->dest]) return idx;
		}
	}

	return -2;
}

template <class Abelian, class Allocator>
std::vector<Abelian> separated_graph<Abelian, Allocator>::query(int p)
{
	int size = 1;
	for(edge* nx=nodes[p]; nx!=NULL; nx=nx->next) ++size;

	//int *root = (int*) al->allocate(size * sizeof(int));
	//Abelian *sol = (Abelian*) al->allocate(size * sizeof(Abelian));

	root[0] = 0;
	sol[0] = value[root_pos[p]] + (-value[p]);
	int idx=1;
	for(edge* nx=nodes[p]; nx!=NULL; nx=nx->next, ++idx){
		root[idx] = 1 + find_child(p, lowlink[nx->dest]);
		if(root[idx] == -1) root[idx] = idx;
		//printf("%d->%d %d %d\n", p, nx->dest, idx, root[idx]);
		sol[idx] = value[nx->dest];
	}

	for(int i=size-1; i>0; --i){
		if(root[i] > i){
			//something is wrong
		}else if(root[i] < i){
			sol[root[i]] = sol[root[i]] + sol[i];
		}
	}

	std::vector<Abelian> ret;
	for(int i=(p == root_pos[p] ? 1 : 0);i<size;i++) if(root[i] == i) ret.push_back(sol[i]);

	//al->release(root);
	//al->release(sol);

	return ret;
}

template <class Abelian, class Allocator>
void separated_graph<Abelian, Allocator>::query(int p, std::vector<Abelian> &ret)
{
	int size = 1;
	for(edge* nx=nodes[p]; nx!=NULL; nx=nx->next) ++size;

	//int *root = (int*) al->allocate(size * sizeof(int));
	//Abelian *sol = (Abelian*) al->allocate(size * sizeof(Abelian));

	root[0] = 0;
	sol[0] = value[root_pos[p]] + (-value[p]);
	int idx=1;
	for(edge* nx=nodes[p]; nx!=NULL; nx=nx->next, ++idx){
		root[idx] = 1 + find_child(p, lowlink[nx->dest]);
		if(root[idx] == -1) root[idx] = idx;
		//printf("%d->%d %d %d\n", p, nx->dest, idx, root[idx]);
		sol[idx] = value[nx->dest];
	}

	for(int i=size-1; i>0; --i){
		if(root[i] > i){
			//something is wrong
		}else if(root[i] < i){
			sol[root[i]] = sol[root[i]] + sol[i];
		}
	}

	ret.clear();
	for(int i=(p == root_pos[p] ? 1 : 0);i<size;i++) if(root[i] == i) ret.push_back(sol[i]);

	//al->release(root);
	//al->release(sol);
}
