// nurikabe.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "main.h"
#include "nurikabe.h"

int problem[7][7] = {
	{-1,  3, -1, -1, -1,  1, -1},
	{-1, -1, -1, -1, -1, -1, -1},
	{ 2, -1, -1,  1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1},
	{-1,  1, -1, -1,  2, -1, -1},
	{-1, -1,  2, -1, -1, -1, -1},
	{ 1, -1, -1, -1,  1, -1,  6},
};

int main(int argc, char* argv[])
{
	//problem_test();
	//graph_test();
	//expand_white_test();
	//solver_frontend();
	pencilbox_frontend();

	return 0;
}

