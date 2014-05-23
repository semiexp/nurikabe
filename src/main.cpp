// nurikabe.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "main.h"
#include "core/nurikabe.h"

int main(int argc, char* argv[])
{
	//brute_test();
	//csp_test();
	problem_test();
	//graph_test();
	//expand_white_test();
	//solver_frontend();
	//pencilbox_frontend(argc, argv);
	nk_field f(5, 5);
	f.set_hint(3, 2, 3);
	f.determine_white(2, 0);
	f.determine_white(2, 1);
	printf("%d\n", f.status());
	f.debug(stdout);

	return 0;
}

