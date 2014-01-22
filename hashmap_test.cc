#include "hashmap.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include "string.h"

using namespace li;
using namespace std;
char buf1[128], buf2[128];

int main ()
{
	hashmap <string, string> _map;
	hashmap <string, string>::iterator it;

	for (u64 i = 0, a = 1; i < 64; ++i, a <<= 1)
	{
		sprintf (buf1, "%llu", i);
		sprintf (buf2, "%llu", a);
		_map[string (buf1)] = string (buf2);
	}
	
	for (u64 i = 0, a = 1; i < 64; ++i, a <<= 1)
	{
		sprintf (buf1, "%llu", i);
		puts (_map[string (buf1)].c_str ());
	}

	_map.erase (_map.find (string ("1")));
	_map.erase (_map.find (string ("6")));
	_map.erase (string ("16"));

	for (it = _map.begin (); it != _map.end (); ++it)
		printf ("1 << %s = %s\n", it->first.c_str (), it->second.c_str());

	return 0;
}
