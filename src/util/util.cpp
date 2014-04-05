
#include "util.h"

#include <string>
#include <conio.h>

int parse_int(std::string s)
{
	int ret = 0;

	for(int i = 0; i < s.size(); i++){
		if(s[i] >= '0' && s[i] <= '9'){
			ret *= 10;
			ret += s[i] - '0';
		}else return -1;
	}

	return ret;
}

int get_onekey()
{
	return _getch();
}
