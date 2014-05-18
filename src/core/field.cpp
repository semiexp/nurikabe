
#include "nurikabe.h"

const int nk_field::ax[] = {-1, 0, 1, 0};
const int nk_field::ay[] = {0, -1, 0, 1};

nk_field::nk_field()
{
	H = W = 0;
	field = NULL;
	t_status = NORMAL;
	t_progress = 0;
	total_wcells = 0;
}

nk_field::nk_field(int Ht, int Wt)
{
	H = Ht; W = Wt;
	field = new cell[H*W];
	t_status = NORMAL;
	t_progress = 0;
	total_wcells = 0;

	for(int i=0;i<H*W;i++) field[i] = cell(UNDECIDED, -1, -1, i);
}

nk_field::nk_field(int Ht, int Wt, int* value)
{
	H = Ht; W = Wt;
	field = new cell[H*W];
	t_status = NORMAL;
	t_progress = 0;
	total_wcells = 0;

	for(int i=0;i<H*W;i++) field[i] = cell(UNDECIDED, -1, -1, i);

	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			if(value[i * W + j] > 0){
				at(i, j).value = WHITE;
				at(i, j).hint = value[i * W + j];
			}
		}


	for(int i = 0; i < H; i++)
		for(int j = 0; j < W; j++){
			if(value[i * W + j] > 0){
				at(i, j).value = NORMAL;
				determine_white(i, j);
			}
		}
}

nk_field::nk_field(const nk_field &src)
{
	H = src.H; W = src.W;
	field = new cell[H*W];
	for(int i=0;i<H*W;i++) field[i] = src.field[i];
	t_status = src.t_status;
	t_progress = src.t_progress;
	total_wcells = src.total_wcells;
}

nk_field &nk_field::operator=(const nk_field &src)
{
	this->~nk_field();

	H = src.H; W = src.W;
	field = new cell[H*W];
	for(int i=0;i<H*W;i++) field[i] = src.field[i];
	t_status = src.t_status;
	t_progress = src.t_progress;
	total_wcells = src.total_wcells;
	
	return *this;
}


nk_field::~nk_field()
{
	if(field != NULL) delete [] field;
}

int nk_field::join(int p, int q)
{
	p = root(p);
	q = root(q);

	if(p == q) return NORMAL;

	if(field[p].value == UNDECIDED || field[p].value != field[q].value){
		return t_status |= INCONSISTENT;
	}

	if(field[p].hint > 0 && field[q].hint > 0){
		return t_status |= INCONSISTENT;
	}

	if(field[q].hint > 0 || (field[p].hint == -1 && field[p].root > field[q].root)){
		// swap (p, q)
		p ^= q;
		q ^= p;
		p ^= q;
	}

	std::vector<int> to_avoid_adj;
	if(field[p].hint > 0 && field[q].hint < 0){
		int q2 = q;
		do{
			to_avoid_adj.push_back(q2);
			q2 = field[q2].next;
		}while(q2 != q);
	}

	field[p].root += field[q].root;
	field[q].root = p;

	field[p].next ^= field[q].next;
	field[q].next ^= field[p].next;
	field[p].next ^= field[q].next;

	for(int i=0;i<to_avoid_adj.size();i++) avoid_adjacent_unions(to_avoid_adj[i] / W, to_avoid_adj[i] % W);

	if(field[p].hint > 0 && field[p].hint < -field[p].root){
		return t_status |= INCONSISTENT;
	}

	if(field[p].hint == -field[p].root){
		//close!
		return close_white(p);
	}

	return NORMAL;
}

int nk_field::close_white(int p)
{
	int ps = p;

	int ret = NORMAL;

	do{
		for(int i=0;i<4;i++){
			int y2 = p/W + ay[i], x2 = p%W + ax[i];
			if(range(y2, x2) && root(id(y2, x2)) != root(p)){
				ret |= determine_black(y2, x2);
			}
		}

		p = field[p].next;
	}while(p != ps);

	return ret;
}

int nk_field::avoid_adjacent_unions(int y, int x)
{
	int y2, x2;
	int ret = NORMAL;

	if(at(root(id(y, x))).hint < 0) return NORMAL;

	for(int i=0;i<4;i++){
		y2 = y + ay[i]*2; x2 = x + ax[i]*2;

		if(range(y2, x2) && at(y2, x2).value == WHITE && at(root(id(y2, x2))).hint > 0 && root(id(y, x)) != root(id(y2, x2))){
			ret |= determine_black(y + ay[i], x + ax[i]);
		}

		y2 = y + ay[i] + ay[(i+1)%4]; x2 = x + ax[i] + ax[(i+1)%4];
		if(range(y2, x2) && at(y2, x2).value == WHITE && at(root(id(y2, x2))).hint > 0 && root(id(y, x)) != root(id(y2, x2))){
			ret |= determine_black(y + ay[i], x + ax[i]);
			ret |= determine_black(y + ay[(i+1)%4], x + ax[(i+1)%4]);
		}
	}

	return ret;
}

int nk_field::avoid_closed_white(int y, int x)
{
	if(at(y, x).value != UNDECIDED) return NORMAL;

	for(int i=0;i<4;i++){
		if(range(y + ay[i], x + ax[i]) && at(y + ay[i], x + ax[i]).value != BLACK) return NORMAL;
	}

	return determine_black(y, x);
}

int nk_field::determine_white(int y, int x)
{
	if(at(y, x).value == WHITE) return NORMAL;
	if(at(y, x).value == BLACK){
		return t_status |= INCONSISTENT;
	}

	++t_progress;
	at(y, x).value = WHITE;
	
	int ret = NORMAL;

	for(int i=0;i<4;i++) if(range(y + ay[i], x + ax[i]) && at(y + ay[i], x + ax[i]).value == WHITE)
		ret |= join(id(y, x), id(y + ay[i], x + ax[i]));

	if(at(y, x).hint == 1){
		return close_white(id(y, x));
	}

	ret |= avoid_adjacent_unions(y, x);

	if(check_complete()) ret |= SOLVED;

	return t_status |= ret; // (* TODO *)
}

int nk_field::determine_black(int y, int x)
{
	if(at(y, x).value == BLACK) return NORMAL;
	if(at(y, x).value == WHITE){
		return t_status |= INCONSISTENT;
	}

	++t_progress;
	at(y, x).value = BLACK;

	for(int i=0;i<4;i++) if(range(y + ay[i], x + ax[i]) && at(y + ay[i], x + ax[i]).value == BLACK)
		join(id(y, x), id(y + ay[i], x + ax[i]));

	int ret = NORMAL;

	for(int i=0;i<4;i++){
		int y2 = y + ay[i], x2 = x + ax[i];

		if(range(y2, x2) && at(y2, x2).value == UNDECIDED) ret |= avoid_closed_white(y2, x2);
	}

	// avoid 2*2 black
	for(int i=0;i<4;i++){
		int y2 = y + ay[i] + ay[(i+1)%4], x2 = x + ax[i] + ax[(i+1)%4];

		if(range(y2, x2)){
			if(at(y2, x2).value == BLACK){
				if(at(y + ay[i], x + ax[i]).value == BLACK) ret |= determine_white(y + ay[(i+1)%4], x + ax[(i+1)%4]);
				if(at(y + ay[(i+1)%4], x + ax[(i+1)%4]).value == BLACK) ret |= determine_white(y + ay[i], x + ax[i]);
			}
			if(at(y + ay[i], x + ax[i]).value == BLACK && at(y + ay[(i+1)%4], x + ax[(i+1)%4]).value == BLACK) ret |= determine_white(y2, x2);
		}
	}

	if(check_complete()) ret |= SOLVED;

	return t_status |= ret;
}

bool nk_field::check_complete()
{
	int black_size = H * W;
	int black_repr = -1;

	if(t_progress != H * W) return false;

	for(int i = 0; i < H; i++){
		for(int j = 0; j < W; j++){
			if(at(i, j).value == UNDECIDED) return false;

			if(at(i, j).value == BLACK){
				black_repr = id(i, j);
			}else{
				if(at(root(id(i, j))).hint < 0){
					return false;
				}

				if(at(i, j).hint > 0){
					black_size -= at(i, j).hint;
					if(at(i, j).root != -at(i, j).hint) return false;
				}
			}
		}
	}

	if(black_repr == -1) return true;

	if(at(root(black_repr)).root != -black_size) return false;

	return true;
}

int nk_field::set_hint(int y, int x, int hint, bool update)
{
	if(!update){
		at(y, x).hint = hint;
		at(y, x).value = WHITE;

		return NORMAL;
	}

	at(y, x).hint = hint;
	return determine_white(y, x);
}

void nk_field::debug(FILE* fp)
{
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			if(at(i, j).value == BLACK) fprintf(fp, "### ");
			else if(at(i, j).value == WHITE){
				if(at(i, j).hint > 0) fprintf(fp, "%3d ", at(i, j).hint);
				else fprintf(fp, "    ");
			}else fprintf(fp, "... ");
		}

		fprintf(fp, "\n");
	}
}

void nk_field::debug2(FILE* fp)
{
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			if(at(i, j).value == BLACK) fprintf(fp, "### ");
			else fprintf(fp, "%3d ", root(id(i, j)));
		}

		fprintf(fp, "\n");
	}
}
