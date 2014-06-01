
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
