
class nk_solver;

#include <vector>
#include <algorithm>

class nk_field
{
	friend class nk_solver;

	static const int ax[4], ay[4];

	struct cell
	{
		int value, hint, root, next;
		// the cells containing a hint should always be root

		cell() {}
		cell(int value, int hint, int root, int next) : value(value), hint(hint), root(root), next(next) {}
	};

	int H, W;
	cell *field;
	int t_status, t_progress;
	int total_wcells;

	bool range(int y, int x) { return 0<=y && y<H && 0<=x && x<W; }
	int id(int y, int x) { return y * W + x; }
	int id_valid(int y, int x) { return range(y, x) ? id(y, x) : -1; }
	cell &at(int y, int x){ return field[id(y, x)]; }
	cell &at(int p){ return field[p]; }

	int root(int p){ return (field[p].root < 0) ? p : (field[p].root = root(field[p].root)); }
	int root_conservative(int p){ return (field[p].root < 0) ? p : root(field[p].root); }
	int join(int p, int q);

	int close_white(int p);
	int avoid_adjacent_unions(int y, int x);
	int avoid_closed_white(int y, int x);

	int determine_white(int y, int x);
	int determine_white(int p){ return determine_white(p / W, p % W); }
	int determine_black(int y, int x);
	int determine_black(int p){ return determine_black(p / W, p % W); }

	bool check_complete();

public:
	static const int UNDECIDED = 0;
	static const int WHITE = 1;
	static const int BLACK = 2;

	static const int NORMAL = 0;
	static const int SOLVED = 1;
	static const int INCONSISTENT = 2;

	nk_field();
	nk_field(int Ht, int Wt);
	nk_field(int Ht, int Wt, int* value);
	nk_field(const nk_field &src);
	~nk_field();

	int set_hint(int y, int x, int hint, bool update=true);

	int board_height() { return H; }
	int board_width() { return W; }

	int cell_hint(int y, int x) { return at(y, x).hint; }
	int cell_value(int y, int x) { return at(y, x).value; }
	int status(){ return t_status; }
	int progress(){ return t_progress; }

	void debug(FILE* fp);
	void debug2(FILE* fp);
};

class nk_solver
{
	struct vector3
	{
		/*
		x: number of cells
		y: number of white (alreadly determined) cells
		z: total value of hints

		conditions that must be satisfied:
			1. z <= x
			2. y <= z
			3. if z is 0, x <= 1
		 */

		/*
		int x, y, z;

		vector3(){ x = y = z = 0; }
		vector3(int x, int y, int z) : x(x), y(y), z(z) {}

		inline vector3 &operator+=(const vector3& v){ x += v.x; y += v.y; z += v.z; return *this; }
		inline vector3 operator+(const vector3& v){ return vector3(x + v.x, y + v.y, z + v.z); }
		inline vector3 operator-(){ return vector3(-x, -y, -z); }
		*/
		int x, y, z, w;

		vector3(){ x = y = z = w = 0; }
		vector3(int x, int y, int z) : x(x), y(y), z(z), w(0) {}
		vector3(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}

		inline vector3 &operator+=(const vector3& v){ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		inline vector3 operator+(const vector3& v){ return vector3(x + v.x, y + v.y, z + v.z, w + v.w); }
		inline vector3 operator-(){ return vector3(-x, -y, -z, -w); }
	};

	static vector3 expand_white_dfs(int y, int x, int H, int W, bool* visited, nk_field &field);

	static const int IMPL_BLACK = 1 << 28;
	static const int IMPL_WHITE = 1 << 29;
	static const int IMPL_MASK = (1 << 27) - 1;

	static int apply_implication_graph(nk_field &field, std::vector<std::pair<int, int> > &graph);
	static std::vector<std::pair<int, int> > assumption_order(nk_field &field);

public:
	static int solve(nk_field &field);

	static int expand_black(nk_field &field);

	static int expand_white(nk_field &field);

	static int check_reachability(nk_field &field);

	static int assumption(nk_field &field, int depth = 1);

	static int solve_csp_old(nk_field &field);
	static int solve_csp(nk_field &field);
};
