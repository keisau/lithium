#include <hashmap.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <lithium/string.h>
#include <algorithm>
#include <map>

using namespace li;
using namespace std;
char buf1[128], buf2[128];

int main ()
{
	hashmap <string, string> _map(1024);
	hashmap <int, int> _map2(3);
	hashmap <string, string>::iterator it;
	map <int, int> _map4;
	map <int, int>::const_iterator cmit = _map4.begin ();

	printf ("_map cap: %d\n",_map.capacity ());
	_map2.insert (1, 1);
	_map = hashmap <string, string> (3);
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
	_map3.erase (_map3.find (string ("1")));
	_map3.erase (_map3.find (string ("6")));
	_map3.erase (string ("0"));
	_map3.erase (string ("1"));
	_map3.erase (string ("2"));
	_map3.erase (string ("3"));
	_map3.erase (string ("4"));
	_map3.erase (string ("5"));
	_map3.erase (string ("16"));
	
	puts ("_map3");
	for (it = _map3.begin (); it != _map3.end (); ++it) {
		printf ("1 << %s = %s\n", it->first.c_str (), it->second.c_str());
	}
	
	if (_map3.empty ()) puts ("empty now");
	else puts ("not empty");
	printf ("_map3 size : %d, %d\n", _map3.capacity (), _map3.size ());
	return 0;
}
