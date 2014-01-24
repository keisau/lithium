#include "hashmap.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include "string.h"
#include <map>

using namespace li;
using namespace std;
char buf1[128], buf2[128];

class A {
public:
	A () {
	}
	void test () const {
		hashmap<int, int>::const_iterator it = _map.begin ();
	}
	hashmap <int, int> _map;
};

int main ()
{
	hashmap <string, string> _map;
	hashmap <int, int> _map2;
	hashmap <string, string>::iterator it;
	map <int, int> _map4;
	map <int, int>::const_iterator cmit = _map4.begin ();
	A a;
	a.test ();

	_map2.insert (1, 1);
	_map = hashmap <string, string> ();
	for (it = _map.begin (); it != _map.end (); ++it) {
		printf ("1 << %s = %s\n", it->first.c_str (), it->second.c_str());
	}

	for (u64 i = 0, a = 1; i < 64; ++i, a <<= 1)
	{
		sprintf (buf1, "%llu", i);
		sprintf (buf2, "%llu", a);
		_map[string (buf1)] = string (buf2);
	}
	
	for (u64 i = 0, a = 1; i < 64; ++i, a <<= 1)
	{
		sprintf (buf1, "%llu", i);
		string str = string (buf1);
		puts (_map[str].c_str ());
		puts (buf1);
	}
	
	hashmap <string, string> _map3 (_map);

	_map.clear();
	if (_map.empty ()) puts ("empty now");
	else puts ("not empty");
	if (_map.find (string ("1")) == _map.end())
		puts ("gd");
	else
		puts ("bad");
	_map.erase (_map.find (string ("1")));
	_map.erase (_map.find (string ("6")));
	_map.erase (string ("0"));
	_map.erase (string ("1"));
	_map.erase (string ("2"));
	_map.erase (string ("3"));
	_map.erase (string ("4"));
	_map.erase (string ("5"));
	_map.erase (string ("16"));

	for (it = _map3.begin (); it != _map3.end (); ++it) {
		printf ("1 << %s = %s\n", it->first.c_str (), it->second.c_str());
	}
	
	if (_map3.empty ()) puts ("empty now");
	else puts ("not empty");
	return 0;
}
