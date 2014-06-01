
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

public:
	separated_graph(){ al = NULL; V = 0; nodes = NULL; root = NULL; sol = NULL; }
	separated_graph(int V_, int E_, Allocator *al_);
	separated_graph(int V_, int E_, Allocator *al_, Abelian *val_);
	~separated_graph();

	void set_value(Abelian *val_){ for(int i=0;i<V;i++) value[i] = val_[i]; }
	void add_edge(int s, int d){ add_edge_sub(s, d); add_edge_sub(d, s); }

	void init();

	template <class Storage>
	void query(int p, Storage &ret);
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
std::vector<Abelian> separated_graph<Abelian, Allocator>::query(int p)
{
	std::vector<Abelian> ret;
	query(p, ret);

	return ret;
}

template <class Abelian, class Allocator>
template <class Storage> 
void separated_graph<Abelian, Allocator>::query(int p, Storage &ret)
{
	int size = 1;
	for(edge* nx=nodes[p]; nx!=NULL; nx=nx->next) ++size;

	ret.clear();

	if(p == root_pos[p]) {
		for(edge* nx = nodes[p]; nx != NULL; nx = nx->next) {
			ret.push_back(value[nx->dest]);
		}
	} else {
		Abelian parent_val = value[root_pos[p]] + (-value[p]);

		for(edge* nx = nodes[p]; nx != NULL; nx = nx->next) {
			if(left[p] <= lowlink[nx->dest] && lowlink[nx->dest] < right[p]) {
				ret.push_back(value[nx->dest]); 
			} else {
				parent_val = parent_val + value[nx->dest];
			}
		}

		ret.push_back(parent_val);
	}
}
