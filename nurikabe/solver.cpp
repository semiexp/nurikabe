
#include "nurikabe.h"
#include "graph.h"

int nk_solver::solve(nk_field &field)
{
	int ret = nk_field::NORMAL;

	int progress_last;

	do{
		progress_last = field.progress();

		ret |= expand_black(field);
		ret |= expand_white2(field);
		ret |= check_reachability(field);

	}while(progress_last != field.progress() && ret == nk_field::NORMAL);

	return ret;
}

int nk_solver::apply_implication_graph(nk_field &field, std::vector<std::pair<int, int> > &graph)
{
	int H = field.H;
	int W = field.W;
	int ret = nk_field::NORMAL;

	int progress_last;

	do{
		progress_last = field.progress();

		for(int i = 0; i < graph.size(); i++){
			int st = field.at(graph[i].first & IMPL_MASK).value;

			if((st == nk_field::BLACK && (graph[i].first & IMPL_BLACK)) || (st == nk_field::WHITE && (graph[i].first & IMPL_WHITE))){
				if(graph[i].second & IMPL_BLACK)
					ret |= field.determine_black(graph[i].second & IMPL_MASK);

				if(graph[i].second & IMPL_WHITE)
					ret |= field.determine_white(graph[i].second & IMPL_MASK);
			}
		}

		if(progress_last == field.progress() || ret == nk_field::NORMAL) break;

		ret |= solve(field);
	}while(progress_last != field.progress() && ret == nk_field::NORMAL);

	return ret;
}

int nk_solver::assumption(nk_field &field, int depth)
{
	int H = field.H;
	int W = field.W;

	int progress_last;
	int ret = nk_field::NORMAL;

	ret |= solve(field);

	bool *impl = new bool[H * W];

	for(int i = 0; i < H * W; i++) impl[i] = false;

	std::vector<std::pair<int, int> > impl_graph;

	do{
		for(int i = 0; i < H * W; i++) impl[i] = false;
		progress_last = field.progress();

		for(int i = 0; i < H; i++){
			for(int j = 0; j < W; j++){
				if(impl[field.id(i, j)]) continue;

				if(field.at(i, j).value == nk_field::UNDECIDED){
					if(i > 0 && j > 0 && i < H - 1 && j < W - 1){
						if(field.at(i-1, j-1).value == nk_field::UNDECIDED &&
						   field.at(i-1, j  ).value == nk_field::UNDECIDED &&
						   field.at(i-1, j+1).value == nk_field::UNDECIDED &&
						   field.at(i  , j-1).value == nk_field::UNDECIDED &&
						   field.at(i  , j+1).value == nk_field::UNDECIDED &&
						   field.at(i+1, j-1).value == nk_field::UNDECIDED &&
						   field.at(i+1, j  ).value == nk_field::UNDECIDED &&
						   field.at(i+1, j+1).value == nk_field::UNDECIDED) continue;
					}

					nk_field f1(field), f2(field);

					f1.determine_black(i, j);
					f2.determine_white(i, j);

					if(depth == 1){
						solve(f1);
						solve(f2);
					}else{
						assumption(f1, depth - 1);
						assumption(f2, depth - 1);
					}

					if((f1.status() & nk_field::INCONSISTENT) && (f2.status() & nk_field::INCONSISTENT)){
						return field.t_status |= nk_field::INCONSISTENT;
					}

					bool updated = false;

					if(f1.status() & nk_field::INCONSISTENT){
						ret |= field.determine_white(i, j);

						updated = true;
					}

					if(f2.status() & nk_field::INCONSISTENT){
						ret |= field.determine_black(i, j);

						updated = true;
					}

					for(int k = 0; k < H; k++){
						for(int l = 0; l < W; l++){
							if(field.at(k, l).value == nk_field::UNDECIDED){
								int v1 = f1.at(k, l).value, v2 = f2.at(k, l).value;
								int p1 = field.id(i, j), p2 = field.id(k, l);

								if(v1 != nk_field::UNDECIDED && v2 != nk_field::UNDECIDED && v1 == v2){
									if(v1 == nk_field::BLACK) ret |= field.determine_black(k, l);
									else ret |= field.determine_white(k, l);

									updated = true;
								}else{
									if(v1 != nk_field::UNDECIDED && v2 != nk_field::UNDECIDED) impl[field.id(k, l)] = true;

									if(v1 == nk_field::WHITE){
										impl_graph.push_back(std::make_pair(IMPL_BLACK | p1, IMPL_WHITE | p2));
										impl_graph.push_back(std::make_pair(IMPL_BLACK | p2, IMPL_WHITE | p1));
									}else if(v1 == nk_field::BLACK){
										impl_graph.push_back(std::make_pair(IMPL_BLACK | p1, IMPL_BLACK | p2));
										impl_graph.push_back(std::make_pair(IMPL_WHITE | p2, IMPL_WHITE | p1));
									}

									if(v2 == nk_field::WHITE){
										impl_graph.push_back(std::make_pair(IMPL_WHITE | p1, IMPL_WHITE | p2));
										impl_graph.push_back(std::make_pair(IMPL_BLACK | p2, IMPL_BLACK | p1));
									}else if(v2 == nk_field::BLACK){
										impl_graph.push_back(std::make_pair(IMPL_WHITE | p1, IMPL_BLACK | p2));
										impl_graph.push_back(std::make_pair(IMPL_WHITE | p2, IMPL_BLACK | p1));
									}
								}
							}
						}
					}

					if(updated){
						ret |= apply_implication_graph(field, impl_graph);
					}
				}
			}
		}
		
		//ret |= solve(field);

	}while(progress_last != field.progress() && ret == nk_field::NORMAL);
	
	delete [] impl;

	return ret;
}

/*
int nk_solver::assumption(nk_field &field, int depth)
{
	int H = field.H;
	int W = field.W;

	int progress_last;
	int ret = nk_field::NORMAL;

	ret |= solve(field);

	bool *impl = new bool[H * W];

	for(int i = 0; i < H * W; i++) impl[i] = false;

	do{
		for(int i = 0; i < H * W; i++) impl[i] = false;
		progress_last = field.progress();

		for(int i = 0; i < H; i++){
			for(int j = 0; j < W; j++){
				if(impl[field.id(i, j)]) continue;

				if(field.at(i, j).value == nk_field::UNDECIDED){
					if(i > 0 && j > 0 && i < H - 1 && j < W - 1){
						if(field.at(i-1, j-1).value == nk_field::UNDECIDED &&
						   field.at(i-1, j  ).value == nk_field::UNDECIDED &&
						   field.at(i-1, j+1).value == nk_field::UNDECIDED &&
						   field.at(i  , j-1).value == nk_field::UNDECIDED &&
						   field.at(i  , j+1).value == nk_field::UNDECIDED &&
						   field.at(i+1, j-1).value == nk_field::UNDECIDED &&
						   field.at(i+1, j  ).value == nk_field::UNDECIDED &&
						   field.at(i+1, j+1).value == nk_field::UNDECIDED) continue;
					}

					nk_field f1(field), f2(field);

					f1.determine_black(i, j);
					f2.determine_white(i, j);

					if(depth == 1){
						solve(f1);
						solve(f2);
					}else{
						assumption(f1, depth - 1);
						assumption(f2, depth - 1);
					}

					if((f1.status() & nk_field::INCONSISTENT) && (f2.status() & nk_field::INCONSISTENT)){
						return field.t_status |= nk_field::INCONSISTENT;
					}

					bool updated = false;

					if(f1.status() & nk_field::INCONSISTENT){
						ret |= field.determine_white(i, j);

						updated = true;
					}

					if(f2.status() & nk_field::INCONSISTENT){
						ret |= field.determine_black(i, j);

						updated = true;
					}

					for(int k = 0; k < H; k++){
						for(int l = 0; l < W; l++){
							if(field.at(k, l).value == nk_field::UNDECIDED && f1.at(k, l).value != nk_field::UNDECIDED && f2.at(k, l).value != nk_field::UNDECIDED){
								if(f1.at(k, l).value == f2.at(k, l).value){
									if(f1.at(k, l).value == nk_field::BLACK) ret |= field.determine_black(k, l);
									else ret |= field.determine_white(k, l);

									updated = true;
								}else{
									impl[field.id(k, l)] = true;
									//printf("(%d, %d) is implicated\n", k, l);
								}
							}
						}
					}

					if(updated) ret |= solve(field);
				}
			}
		}
		
		ret |= solve(field);

	}while(progress_last != field.progress() && ret == nk_field::NORMAL);
	
	delete [] impl;

	return ret;
}
*/
