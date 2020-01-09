#include "share.h"

#include <iostream>
#include <string>

#include "nickname.h"

using std::cout;
using std::endl;
using std::string;

int main() {

	RadixTrie<> rtree;
	rtree.append("mark");
	rtree.append("mast");
	rtree.printTree();
	rtree.print();
	return 0;
}
