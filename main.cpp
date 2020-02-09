#include "share.h"

#include <iostream>

#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <vector>

#include "nickname.h"

using std::cout;
using std::endl;
using std::string;

//std::locale utf8_locale = std::locale(std::locale{}, new std::codecvt_utf8<wchar_t>{});
//
//struct InOutInit {
//	
//	void init(std::wostream& out = std::wcout) {
//		out.imbue(utf8_locale);
//	}
//};

int main() {;
	//InOutInit inOutInit;
	//inOutInit.init();
	RadixTrie rtree;
	/*rtree.isOutQuotes = false;*/
	rtree.isOutQuotes = true;
	/*rtree.isOutSpecForDeps = false;*/
	rtree.isOutSpecForDeps = true;
	//rtree.isUpFstCharOnPrint = true; // deprecated
	rtree.isOutCodesInPrintTree = true;
	rtree.sGap = "  ";

	// TODO!!!!! Добавить обработку файла данных и потока ввода 
	std::vector names = {
		u8"aleksey",
		u8"sasha",
		u8"aleks",
		u8"alek",
		u8"alesha",
		u8"maksim",
		u8"Мxz",
		u8"Маша",
		u8"Мама",
		u8"АЛЕША",
		u8"Алексей",
		u8"Ыва"
	};

	for (auto name : names) {
		rtree.append(name);
	}

	rtree.print();
	wcout << endl;
	rtree.printTree();

	return 0;
}
