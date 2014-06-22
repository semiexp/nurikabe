
#include <string>

int parse_int(std::string in);

int get_onekey();

template <class T, int SIZE>
class mini_vector
{
	T val[SIZE];
	int pos;
	
public:
	mini_vector() { pos = 0; }
	void push_back(T &v) { val[pos++] = v; }

	T &operator[](int idx) { return val[idx]; }
	int size() { return pos; }
	void clear() { pos = 0; }
};

class union_find
{
	int *hd;
	int N;

public:
	union_find(int N) { initialize(N); }
	union_find() { hd = NULL; }
	~union_find() { if (!hd) delete [] hd; }

	void initialize(int N_) {
		N = N_;

		hd = new int[N];

		for (int i = 0; i < N; i++) hd[i] = -1;
	}

	int root(int p) { return (hd[p] < 0 ? p : (hd[p] = root(hd[p]))); }
	bool join(int p, int q) {
		p = root(p); q = root(q);

		if(p == q) return false;

		hd[p] += hd[q];
		hd[q] = p;
	}
};

template <class T>
class valued_union_find : public union_find
{
	T *data;

public:
	valued_union_find() { hd = NULL; data = NULL; }
	~valued_union_find() {
		if (!hd) delete [] hd; 
		if (!data) delete [] data;
	}

	void initialize(int N_) {
		N = N_;

		hd = new int[N];
		data = new T[N];

		for (int i = 0; i < N; i++) hd[i] = -1;
	}

	bool join(int p, int q) {
		p = root(p); q = root(q);

		if(p == q) return false;

		hd[p] += hd[q];
		data[p] = data[p] + data[q];
		hd[q] = p;
	}

	void increase(int p, const T& val)
	{
		p = root(p);
		data[p] = data[p] + val;
	}

	T get_union_value(int p)
	{
		return data[p];
	}
};
