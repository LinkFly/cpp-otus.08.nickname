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

struct InOutInit {
	std::locale utf8_locale = std::locale(std::locale{}, new std::codecvt_utf8<wchar_t>{});
	void init(std::wostream& out = std::wcout) {
		out.imbue(utf8_locale);
	}
};

int main() {
#if defined(WIN32) && defined(AUTO_CHANGE_CODEPAGE_FOR_WIN)
	systemSpecificInOutInit();
#endif
	InOutInit inOutInit;
	inOutInit.init();
	
	RadixTrie rtree;
	rtree.isOutQuotes = false;
	rtree.isOutSpecForDeps = false;
	rtree.sGap = L"  ";

	// TODO!!!!! Добавить обработку файла данных и потока ввода 
	std::vector names = {
		L"aleksey",
		L"sasha",
		L"aleks",
		L"alek",
		L"alesha",
		L"maksim"
	};

	for (auto name : names) {
		rtree.append(name);
	}

	rtree.print();
	wcout << endl;
	rtree.printTree();

	return 0;
}
