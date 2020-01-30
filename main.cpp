#include "share.h"

#include <iostream>
#include <string>
#include <locale>

// tmp
//#include <bitset>

#include "nickname.h"

using std::cout;
using std::endl;
using std::string;

int main() {
	setlocale(LC_ALL, "Russian");
	RadixTrie rtree;
	rtree.append(L"mark");
	rtree.append(L"mast");
	rtree.append(L"Марк");
	/*rtree.append(L"Марк");
	rtree.append(L"Mark");
	rtree.append(L"Марк");
	rtree.append(L"Маст");*/
	
	rtree.printTree();
	rtree.print();
	

	/*string str{ "Данил" };
	cout << "!!!!!!!!!!!!!!!!!!  " << str[0] << str[2] << " " << std::bitset<8>((int)(unsigned char)(str[2])) << " " << std::bitset<8>((int)(unsigned char)(str[3])) << endl;
	cout << "!!!!!:|" << (int)RadixTrie<>::getMagicUTF8CyryllicStartByte() << "|\n";*/
	return 0;
}
