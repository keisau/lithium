#include <hlist.h>
#include <stdio.h>
#include <string>
#include <lithium/string.h>
using namespace std;
using namespace li;

struct test {
	string		name;
	hlist_node	head;
	test () {}
	test (const char *name) : name (string (name)) {}
	test (string name) : name (name) {}
};

int main ()
{
	test a ("hello"), b ("world");
	hlist_head root;
	root.insert (&a.head);
	root.insert (&b.head);
	for (hlist_node *head = root.head; head; head = head->next) {
		test *tmp = container_of (head, test, head);
		printf ("%s: %p\n",tmp->name.c_str(), &tmp->head);
	}

	return 0;
}
