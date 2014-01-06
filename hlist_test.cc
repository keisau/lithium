#include "hlist.h"
#include <stdio.h>
#include <string>
using namespace std;
using namespace li;

int main ()
{
	hlist <string> lst;
	lst.insert (string ("World"));
	lst.insert (string ("Hello"));
	for (hlist <string>::iterator it = lst.begin ();
			it != lst.end ();
			++it)
		puts (it->c_str());

	return 0;
}
