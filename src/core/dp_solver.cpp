
#include "nurikabe.h"
#include <map>
#include <set>

struct unit_allocator
{
	char *pool;
	std::size_t pTop, pSize;

	unit_allocator() {
		pool = nullptr;
		pSize = 0;
		pTop = 0;
	}

	unit_allocator(std::size_t N)
	{
		pool = new char[N];
		pSize = N;
		pTop = 0;
	}

	~unit_allocator() {
		if (pool) delete[] pool;
	}

	void* allocate(int sz) {
		if (pTop + sz > pSize) {
			fprintf(stderr, "Memory limit exceeded (>_<)");
			exit(1);
			return nullptr;
		}

		pTop += sz;

		return &(pool[pTop - sz]);
	}
};

template <class Allocator>
struct dp_status
{
	typedef short sInt;

	static const sInt BLACK = 0x7fff;

	Allocator *al;
	sInt *uf, *value;
	dp_status *orig1, *orig2;
	int N, prevCol;
	sInt nIsolate;

	dp_status() {
		N = 0;
		nIsolate = 0;
		uf = value = nullptr;
		orig1 = orig2 = nullptr;
	}

	dp_status(int N_, Allocator *al_) {
		N = N_;
		al = al_;
		
		uf = (sInt*)al->allocate(N * sizeof(sInt));
		value = (sInt*)al->allocate(N * sizeof(sInt));
		prevCol = 0;
		nIsolate = 0;

		for (int i = 0; i < N; i++) {
			uf[i] = -1;
			value[i] = 0;
		}
	}

	dp_status(const dp_status& bas) {
		al = bas.al;
		N = bas.N;

		uf = (sInt*)al->allocate(N * sizeof(sInt));
		value = (sInt*)al->allocate(N * sizeof(sInt));
		prevCol = bas.prevCol;
		nIsolate = bas.nIsolate;

		for (int i = 0; i < N; i++) {
			uf[i] = bas.uf[i];
			value[i] = bas.value[i];
		}
	}

	void set_origin(dp_status *orig) {
		if (orig1 == nullptr) orig1 = orig;
		else {
			if (orig2 != nullptr) return;

			if (!(*orig1 == *orig)) orig2 = orig;
		}
	}

	int root(int p) {
		return (uf[p] < 0 ? p : uf[p] = root(uf[p]));
	}

	int safe_root(int p) {
		return uf[p] < 0 ? p : safe_root(uf[p]);
	}

	sInt& pValue(int p) { return value[safe_root(p)]; }

	bool join(int p, int q) {
		p = root(p);
		q = root(q);

		if (p == q) return false;

		uf[p] += uf[q];

		if (value[p] == BLACK || value[q] == BLACK) value[p] = BLACK;
		else value[p] += value[q];
		value[q] = 0;
		uf[q] = p;

		return true;
	}

	void canonize() {
		for (int i = 0; i < N; i++) root(i);

		for (int i = 0; i < N; i++) if (i < root(i)) {
			int p = root(i);

			uf[i] = uf[p];
			uf[p] = i;
			value[i] = value[p];
			value[p] = 0;
		}

		for (int i = 0; i < N; i++) root(i);
	}

	bool is_isolated(int p) {
		if (safe_root(p) == p) {
			int cand = -1;

			for (int i = 0; i < N; i++) if (i != p && safe_root(i) == p) {
				cand = i;
				break;
			}

			return cand == -1;
		}
		return false;
	}

	bool isolate(int p) {
		if (root(p) == p) {
			int cand = -1;
			
			for (int i = 0; i < N; i++) if (i != p && root(i) == p) {
				cand = i;
				break;
			}

			if (cand == -1) return true;

			for (int i = 0; i < N; i++) if (uf[i] == p) uf[i] = cand;
			uf[cand] = uf[p];
			uf[p] = -1;

			value[cand] = value[p];
			value[p] = 0;
		}
		else {
			uf[p] = -1;
			value[p] = 0;
		}

		return false;
	}

	inline bool operator==(const dp_status &st) const {
		if (N != st.N) return false;
		if (prevCol != st.prevCol) return false;

		for (int i = 0; i < N; i++) {
			if (uf[i] != st.uf[i]) return false;
			if (value[i] != st.value[i]) return false;
		}
		return true;
	}

	inline bool operator<(const dp_status &st) const {
		if (N < st.N) return true;
		if (N > st.N) return false;
		if (prevCol < st.prevCol) return true;
		if (prevCol > st.prevCol) return false;

		for (int i = 0; i < N; i++) {
			if (uf[i] < st.uf[i]) return true;
			if (uf[i] > st.uf[i]) return false;

			if (value[i] < st.value[i]) return true;
			if (value[i] > st.value[i]) return false;
		}

		return false;
	}

	void debug(FILE* fp) {
		fprintf(fp, "pr: %d, is: %d, ", prevCol, (int)nIsolate);
		for (int i = 0; i < N; i++) {
			fprintf(fp, "%d(%d) ", root(i), pValue(i));
		}
		fprintf(fp, "\n");
	}
};

bool connectable(int x, int y)
{
	if (x == dp_status<void>::BLACK) return y == dp_status<void>::BLACK;
	if (x == 0 || y == 0) return false;
	if (x > 0 && y > 0) return false;
	if (x < 0 && y < 0) return true;

	return x + y >= 0;
}

bool connectable(int x, int y, int z)
{
	if (x == dp_status<void>::BLACK) return y == dp_status<void>::BLACK && z == dp_status<void>::BLACK;

	return connectable(x, y) && connectable(x + y, z);
}

int nk_solver::solve_dp(nk_field &field)
{
	int H = field.H;
	int W = field.W;

	int t = 0;

	typedef dp_status <unit_allocator> St_type;

	struct ptr_comp
	{
		inline bool operator()(const St_type* left, const St_type* right) { return *left < *right; }
	};

	typedef std::set < St_type*, ptr_comp> Mp_type;

	Mp_type Mp[2] = {
		Mp_type(ptr_comp()),
		Mp_type(ptr_comp())
	};

	unit_allocator al((std::size_t)6 * 1024 * 1024 * 1024); //512MB

	dp_status<unit_allocator> *bas = new (al.allocate(sizeof(St_type))) St_type(W, &al);
	Mp[t].insert(bas);

	int req_sum = 0;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			--req_sum;
			if (field.at(i, j).hint > 0) req_sum += field.at(i, j).hint;
		}
	}

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			fprintf(stderr, "%d %d\n", i, j);
			Mp[1 - t].clear();

			for (St_type* stat : Mp[t]) {
				// check validity
				int cnt = 0;
				for (int k = 0; k < W; k++) if (stat->root(k) == k) {
					if (stat->pValue(k) != St_type::BLACK) cnt += stat->pValue(k);
				}

				if (cnt + req_sum > 0) {
					continue;
				}

				//stat->debug(stderr);
				if (field.at(i, j).value != nk_field::BLACK) {
					// make (i, j) white

					bool valid = true;

					int cVal = ((field.at(i, j).hint > 0) ? (field.at(i, j).hint - 1) : -1);
					if (i > 0) {
						// check validity

						if (stat->pValue(j) == St_type::BLACK) {
							if (stat->is_isolated(j) && stat->nIsolate) valid = false;
						}
						else {
							if (!connectable(cVal, stat->pValue(j))) valid = false;
							cVal += stat->pValue(j);
						}
					}

					if (j > 0) {
						if (stat->pValue(j-1) != St_type::BLACK && stat->root(j) != stat->root(j-1)) {
							if (!connectable(cVal, stat->pValue(j-1))) valid = false;
							cVal += stat->pValue(j-1);
						}
					}

					if (valid) {
						dp_status<unit_allocator> *stat2 = new (al.allocate(sizeof(St_type))) St_type(*stat);

						stat2->prevCol = stat2->pValue(j);

						if (stat2->pValue(j) == St_type::BLACK) {
							if(stat2->isolate(j)) ++(stat2->nIsolate);
						}

						//fprintf(stderr, "cval: %d\n", cVal);
						if (j > 0 && stat2->pValue(j - 1) != St_type::BLACK) stat2->join(j, j - 1);
						stat2->pValue(j) = cVal;

						stat2->canonize();
						
						//fprintf(stderr, "(next) ");
						//stat2->debug(stderr);

						//append
						auto ex = Mp[1 - t].find(stat2);

						if (ex != Mp[1 - t].end()) {
							(*ex)->set_origin(stat);
						}
						else {
							stat2->set_origin(stat);
							Mp[1 - t].insert(stat2);
						}
					}
				}

				if (field.at(i, j).value != nk_field::WHITE && !(i > 0 && j > 0 && stat->pValue(j) == St_type::BLACK && stat->pValue(j-1) == St_type::BLACK && stat->prevCol == St_type::BLACK)) {
					// make (i, j) black
					bool valid = true;

					if (i > 0) {
						// check validity

						if (stat->pValue(j) != St_type::BLACK) {
							if (stat->is_isolated(j) && stat->pValue(j) != 0) valid = false;
						}
					}

					if (valid) {
						dp_status<unit_allocator> *stat2 = new (al.allocate(sizeof(St_type))) St_type(*stat);

						stat2->prevCol = stat2->pValue(j);

						if (stat2->pValue(j) != St_type::BLACK) {
							stat2->isolate(j);
						}

						if (j > 0 && stat2->pValue(j - 1) == St_type::BLACK) stat2->join(j, j - 1);
						stat2->pValue(j) = St_type::BLACK;

						stat2->canonize();

						//fprintf(stderr, "(next) ");
						//stat2->debug(stderr);

						//append
						auto ex = Mp[1 - t].find(stat2);

						if (ex != Mp[1 - t].end()) {
							(*ex)->set_origin(stat);
						}
						else {
							stat2->set_origin(stat);
							Mp[1 - t].insert(stat2);
						}
					}
				}
			}
			t = 1 - t;

			++req_sum;
			if (field.at(i, j).hint > 0) req_sum -= field.at(i, j).hint;
		}
	}

	for (St_type *stat : Mp[t]) {
		int bUnits = stat->nIsolate;

		for (int i = 0; i < W; i++) {
			int v = stat->pValue(i);

			if (v == St_type::BLACK) {
				if (stat->root(i) == i) ++bUnits;
			}
			else if (v != 0) {
				goto nex;
			}
		}

		if (bUnits <= 1) {
			fprintf(stderr, "valid solution:\n");

			while (stat != bas) {
				stat->debug(stderr);
				stat = stat->orig1;
			}
		}
	nex:
		continue;
	}
	return 0;

}
