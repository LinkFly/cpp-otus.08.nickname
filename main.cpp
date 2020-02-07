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
	rtree.isOutQuotes = false;
	rtree.isOutSpecForDeps = false;
	//rtree.isUpFstCharOnPrint = true;
	rtree.sGap = "  ";

	// TODO!!!!! Добавить обработку файла данных и потока ввода 
	std::vector names = {
		L"aleksey",
		L"sasha",
		L"aleks",
		L"alek",
		L"alesha",
		L"maksim",
		//L"Мxz",
		//L"Маша",
		//L"АЛЕША",
		//L"Алексей",
		//L"Ыва"
	};

	for (auto name : names) {
		rtree.append(name);
	}

	rtree.print();
	wcout << endl;
	rtree.printTree();

	return 0;
}
